#include "downloader/image-downloader.h"
#include <QUuid>
#include "extension-rotator.h"
#include "file-downloader.h"
#include "functions.h"
#include "logger.h"
#include "models/filename.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


static void addMd5(Profile *profile, const QString &path)
{
	QCryptographicHash hash(QCryptographicHash::Md5);

	QFile f(path);
	f.open(QFile::ReadOnly);
	hash.addData(&f);
	f.close();

	profile->addMd5(hash.result().toHex(), path);
}


ImageDownloader::ImageDownloader(QSharedPointer<Image> img, QString filename, QString path, int count, bool addMd5, bool startCommands, bool getBlacklisted, QObject *parent, bool loadTags, bool rotate, bool force)
	: QObject(parent), m_image(std::move(img)), m_fileDownloader(false, this), m_filename(std::move(filename)), m_path(std::move(path)), m_loadTags(loadTags), m_count(count), m_addMd5(addMd5), m_startCommands(startCommands), m_getBlacklisted(getBlacklisted), m_writeError(false), m_rotate(rotate), m_force(force)
{}

ImageDownloader::ImageDownloader(QSharedPointer<Image> img, QStringList paths, int count, bool addMd5, bool startCommands, bool getBlacklisted, QObject *parent, bool rotate, bool force)
	: QObject(parent), m_image(std::move(img)), m_fileDownloader(false, this), m_loadTags(false), m_paths(std::move(paths)), m_count(count), m_addMd5(addMd5), m_startCommands(startCommands), m_getBlacklisted(getBlacklisted), m_writeError(false), m_rotate(rotate), m_force(force)
{}

ImageDownloader::~ImageDownloader()
{
	if (m_reply != nullptr)
		m_reply->deleteLater();
}

bool ImageDownloader::isRunning() const
{
	return m_reply != nullptr && m_reply->isRunning();
}

void ImageDownloader::save()
{
	// If we use direct saving or don't want to load tags, we directly save the image
	int needTags = Filename(m_filename).needExactTags(m_image->parentSite());
	bool filenameNeedTags = needTags == 2 || (needTags == 1 && m_image->hasUnknownTag());
	bool blacklistNeedTags = m_getBlacklisted && m_image->tags().isEmpty();
	if (!blacklistNeedTags && (!m_loadTags || !m_paths.isEmpty() || !filenameNeedTags))
	{
		loadedSave();
		return;
	}

	connect(m_image.data(), &Image::finishedLoadingTags, this, &ImageDownloader::loadedSave);
	m_image->loadDetails();
}

void ImageDownloader::abort()
{
	if (m_reply != nullptr && m_reply->isRunning())
		m_reply->abort();
}

void ImageDownloader::loadedSave()
{
	Profile *profile = m_image->parentSite()->getSource()->getProfile();

	// Get the download path from the image if possible
	if (m_paths.isEmpty())
	{
		m_paths = m_image->path(m_filename, m_path, m_count, true, true, true, true);

		// Use a random temporary file if we need the MD5 or equivalent
		if (Filename(m_filename).needTemporaryFile(m_image->tokens(profile)))
		{
			const QString tmpDir = !m_path.isEmpty() ? m_path : QDir::tempPath();
			m_temporaryPath = tmpDir + "/" + QUuid::createUuid().toString().mid(1, 36) + ".tmp";
		}
	}

	// Directly use the image path as temporary file  if possible
	if (m_temporaryPath.isEmpty())
	{ m_temporaryPath = m_paths.first() + ".tmp"; }

	// Check if the image is blacklisted
	if (!m_getBlacklisted)
	{
		const QStringList &detected = profile->getBlacklist().match(m_image->tokens(profile));
		if (!detected.isEmpty())
		{
			log(QStringLiteral("Image contains blacklisted tags: '%1'").arg(detected.join("', '")), Logger::Info);
			emit saved(m_image, makeMap(m_paths, Image::SaveResult::Blacklisted));
			return;
		}
	}

	// Try to save the image if it's already loaded or exists somewhere else on disk
	if (!m_force)
	{
		// Check if the destination files already exist
		bool allExists = true;
		for (const QString &path : qAsConst(m_paths))
		{
			if (!QFile::exists(path))
			{
				allExists = false;
				break;
			}
		}
		if (allExists)
		{
			log(QStringLiteral("File already exists: <a href=\"file:///%1\">%1</a>").arg(m_paths.first()), Logger::Info);
			for (const QString &path : qAsConst(m_paths))
			{ addMd5(profile, path); }
			emit saved(m_image, makeMap(m_paths, Image::SaveResult::AlreadyExistsDisk));
			return;
		}

		// If we don't need any loading, we can return already
		Image::SaveResult res = m_image->preSave(m_temporaryPath);
		if (res != Image::SaveResult::NotLoaded)
		{
			QMap<QString, Image::SaveResult> result {{ m_temporaryPath, res }};

			if (res == Image::SaveResult::Saved || res == Image::SaveResult::Copied || res == Image::SaveResult::Moved || res == Image::SaveResult::Linked)
			{ result = postSaving(res); }

			emit saved(m_image, result);
			return;
		}
	}

	m_url = m_image->url(Image::Size::Full);
	log(QStringLiteral("Loading and saving image in <a href=\"file:///%1\">%1</a>").arg(m_paths.first()));
	loadImage();
}

void ImageDownloader::loadImage()
{
	connect(&m_fileDownloader, &FileDownloader::success, this, &ImageDownloader::success, Qt::UniqueConnection);
	connect(&m_fileDownloader, &FileDownloader::networkError, this, &ImageDownloader::networkError, Qt::UniqueConnection);
	connect(&m_fileDownloader, &FileDownloader::writeError, this, &ImageDownloader::writeError, Qt::UniqueConnection);

	// Delete previous replies for retries
	if (m_reply != nullptr)
	{ m_reply->deleteLater(); }

	// Load the image directly on the disk
	Site *site = m_image->parentSite();
	m_reply = site->get(site->fixUrl(m_url.toString()), m_image->page(), QStringLiteral("image"), m_image.data());
	m_reply->setParent(this);
	connect(m_reply, &QNetworkReply::downloadProgress, this, &ImageDownloader::downloadProgressImage);

	// Create download root directory
	const QString rootDir = m_temporaryPath.section(QDir::separator(), 0, -2);
	if (!QDir(rootDir).exists() && !QDir().mkpath(rootDir))
	{
		log(QStringLiteral("Impossible to create the destination folder: %1.").arg(rootDir), Logger::Error);
		emit saved(m_image, makeMap(m_paths, Image::SaveResult::Error));
		return;
	}

	// If we can't start writing for some reason, return an error
	if (!m_fileDownloader.start(m_reply, QStringList() << m_temporaryPath))
	{
		log(QStringLiteral("Unable to open file"), Logger::Error);
		emit saved(m_image, makeMap(m_paths, Image::SaveResult::Error));
		return;
	}
}

void ImageDownloader::downloadProgressImage(qint64 v1, qint64 v2)
{
	if (m_image->fileSize() == 0 || m_image->fileSize() < v2 / 2)
		m_image->setFileSize(v2);

	emit downloadProgress(m_image, v1, v2);
}

QMap<QString, Image::SaveResult> ImageDownloader::makeMap(const QStringList &keys, Image::SaveResult value)
{
	QMap<QString, Image::SaveResult> res;
	for (const QString &key : keys)
		res.insert(key, value);
	return res;
}

void ImageDownloader::writeError()
{
	emit saved(m_image, makeMap(m_paths, Image::SaveResult::Error));
}

void ImageDownloader::networkError(QNetworkReply::NetworkError error, const QString &msg)
{
	if (error == QNetworkReply::ContentNotFoundError)
	{
		QSettings *settings = m_image->parentSite()->getSource()->getProfile()->getSettings();
		ExtensionRotator *extensionRotator = m_image->extensionRotator();

		const bool sampleFallback = settings->value("Save/samplefallback", true).toBool();
		QString newext = extensionRotator != nullptr ? extensionRotator->next() : QString();

		const bool shouldFallback = sampleFallback && !m_image->url(Image::Size::Sample).isEmpty();
		const bool isLast = newext.isEmpty() || (shouldFallback && m_tryingSample);

		if (m_rotate && (!isLast || (shouldFallback && !m_tryingSample)))
		{
			if (isLast)
			{
				m_url = m_image->url(Image::Size::Sample);
				m_tryingSample = true;
				log(QStringLiteral("Image not found. New try with its sample..."));
			}
			else
			{
				m_url = setExtension(m_image->url(Image::Size::Full), newext);
				log(QStringLiteral("Image not found. New try with extension %1 (%2)...").arg(newext, m_url.toString()));
			}

			m_image->setUrl(m_url);
			loadImage();
		}
		else
		{
			log(QStringLiteral("Image not found."));
			emit saved(m_image, makeMap(m_paths, Image::SaveResult::NotFound));
		}
	}
	else if (error != QNetworkReply::OperationCanceledError)
	{
		log(QStringLiteral("Network error for the image: <a href=\"%1\">%1</a>: %2 (%3)").arg(m_image->url().toString().toHtmlEscaped()).arg(error).arg(msg), Logger::Error);
		emit saved(m_image, makeMap(m_paths, Image::SaveResult::NetworkError));
	}
}

void ImageDownloader::success()
{
	// Handle network redirects
	QUrl redir = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_url = redir;
		loadImage();
		return;
	}

	emit saved(m_image, postSaving());
}

QMap<QString, Image::SaveResult> ImageDownloader::postSaving(Image::SaveResult saveResult)
{
	Profile *profile = m_image->parentSite()->getSource()->getProfile();
	m_image->setSavePath(m_temporaryPath);

	if (!m_filename.isEmpty())
	{ m_paths = m_image->path(m_filename, m_path, m_count, true, true, true, true); }

	QString suffix;
	#ifdef Q_OS_WIN
		if (saveResult == Image::SaveResult::Linked)
		{ suffix = ".lnk"; }
	#endif

	QFile tmp(m_temporaryPath + suffix);
	bool moved = false;

	QMap<QString, Image::SaveResult> result;
	for (const QString &file : qAsConst(m_paths))
	{
		const QString path = file + suffix;

		// Don't overwrite already existing files
		if (QFile::exists(file) || (!suffix.isEmpty() && QFile::exists(path)))
		{
			log(QStringLiteral("File already exists: <a href=\"file:///%1\">%1</a>").arg(file), Logger::Info);
			if (suffix.isEmpty())
			{ addMd5(profile, file); }
			result[path] = Image::SaveResult::AlreadyExistsDisk;
			continue;
		}

		if (!moved)
		{
			const QString dir = path.section(QDir::separator(), 0, -2);
			if (!QDir(dir).exists() && !QDir().mkpath(dir))
			{
				log(QStringLiteral("Impossible to create the destination folder: %1.").arg(dir), Logger::Error);
				result[path] = Image::SaveResult::Error;
				continue;
			}

			tmp.rename(path);
			moved = true;
		}
		else
		{ tmp.copy(path); }

		if (!result.contains(path))
		{ result[path] = saveResult; }

		m_image->postSave(path, result[path], m_addMd5, m_startCommands, m_count);
	}

	if (!moved)
	{ tmp.remove(); }

	return result;
}
