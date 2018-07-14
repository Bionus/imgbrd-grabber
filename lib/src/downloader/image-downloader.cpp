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


ImageDownloader::ImageDownloader(QSharedPointer<Image> img, QString filename, QString path, int count, bool addMd5, bool startCommands, QObject *parent, bool loadTags, bool rotate, bool force)
	: QObject(parent), m_image(std::move(img)), m_fileDownloader(this), m_filename(std::move(filename)), m_path(std::move(path)), m_loadTags(loadTags), m_count(count), m_addMd5(addMd5), m_startCommands(startCommands), m_writeError(false), m_rotate(rotate), m_force(force)
{}

ImageDownloader::ImageDownloader(QSharedPointer<Image> img, QStringList paths, int count, bool addMd5, bool startCommands, QObject *parent, bool rotate, bool force)
	: QObject(parent), m_image(std::move(img)), m_fileDownloader(this), m_loadTags(false), m_paths(std::move(paths)), m_count(count), m_addMd5(addMd5), m_startCommands(startCommands), m_writeError(false), m_rotate(rotate), m_force(force)
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
	if (!m_loadTags || !m_paths.isEmpty() || !Filename(m_filename).needExactTags(m_image->parentSite()))
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
	// Get the download path from the image if possible
	if (m_paths.isEmpty())
	{
		m_paths = m_image->path(m_filename, m_path, m_count, true, false, true, true, true);

		// Use a random temporary file if we need the MD5 or equivalent
		Profile *profile = m_image->parentSite()->getSource()->getProfile();
		if (Filename(m_filename).needTemporaryFile(m_image->tokens(profile)))
		{
			const QString tmpDir = !m_path.isEmpty() ? m_path : QDir::tempPath();
			m_temporaryPath = tmpDir + "/" + QUuid::createUuid().toString().mid(1, 36) + ".tmp";
		}
	}

	// Directly use the image path as temporary file  if possible
	if (m_temporaryPath.isEmpty())
	{ m_temporaryPath = m_paths.first() + ".tmp"; }

	// Try to save the image if it's already loaded or exists somewhere else on disk
	if (!m_force)
	{
		Image::SaveResult res = m_image->preSave(m_temporaryPath);

		// If we don't need any loading, we can return already
		if (res != Image::SaveResult::NotLoaded)
		{
			QMap<QString, Image::SaveResult> result {{ m_temporaryPath, res }};

			if (res == Image::SaveResult::Saved || res == Image::SaveResult::Copied || res == Image::SaveResult::Moved)
			{ result = postSaving(result); }

			emit saved(m_image, result);
			return;
		}
	}

	m_url = m_image->url(Image::Size::Full);
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
	log(QStringLiteral("Loading and saving image in <a href=\"file:///%1\">%1</a>").arg(m_paths.first()));
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
				m_url = setExtension(m_url, newext);
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

QMap<QString, Image::SaveResult> ImageDownloader::postSaving(QMap<QString, Image::SaveResult> result)
{
	m_image->setSavePath(m_temporaryPath);

	if (!m_filename.isEmpty())
	{ m_paths = m_image->path(m_filename, m_path, m_count, true, false, true, true, true); }

	QFile tmp(m_temporaryPath);
	bool moved = false;

	for (int i = 0; i < m_paths.count(); ++i)
	{
		const QString &path = m_paths[i];

		QFile f(path);
		if (f.exists())
		{
			result[path] = Image::SaveResult::AlreadyExists;
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
		{ result[path] = Image::SaveResult::Saved; }

		m_image->postSaving(path, m_addMd5, m_startCommands, m_count, false);
	}

	if (!moved)
	{ tmp.remove(); }

	return result;
}
