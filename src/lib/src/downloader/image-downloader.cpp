#include "downloader/image-downloader.h"
#include <QDir>
#include <QFile>
#include <QImageReader>
#include <QSettings>
#include <QSize>
#include <QUuid>
#include <utility>
#include "extension-rotator.h"
#include "file-downloader.h"
#include "functions.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/filename.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "network/network-reply.h"


static void addMd5(Profile *profile, const QString &path)
{
	QCryptographicHash hash(QCryptographicHash::Md5);

	QFile f(path);
	f.open(QFile::ReadOnly);
	hash.addData(&f);
	f.close();

	profile->addMd5(hash.result().toHex(), path);
}


ImageDownloader::ImageDownloader(Profile *profile, QSharedPointer<Image> img, QString filename, QString path, int count, bool addMd5, bool startCommands, QObject *parent, bool loadTags, bool rotate, bool force, Image::Size size, bool postSave, bool forceExisting)
	: QObject(parent), m_profile(profile), m_image(std::move(img)), m_fileDownloader(false, this), m_filename(std::move(filename)), m_path(std::move(path)), m_loadTags(loadTags), m_count(count), m_addMd5(addMd5), m_startCommands(startCommands), m_writeError(false), m_rotate(rotate), m_force(force), m_postSave(postSave), m_forceExisting(forceExisting)
{
	setSize(size);
}

ImageDownloader::ImageDownloader(Profile *profile, QSharedPointer<Image> img, QStringList paths, int count, bool addMd5, bool startCommands, QObject *parent, bool rotate, bool force, Image::Size size, bool postSave, bool forceExisting)
	: QObject(parent), m_profile(profile), m_image(std::move(img)), m_fileDownloader(false, this), m_loadTags(false), m_paths(std::move(paths)), m_count(count), m_addMd5(addMd5), m_startCommands(startCommands), m_writeError(false), m_rotate(rotate), m_force(force), m_postSave(postSave), m_forceExisting(forceExisting)
{
	setSize(size);
}

ImageDownloader::~ImageDownloader()
{
	if (m_reply != nullptr) {
		m_reply->deleteLater();
	}
}

bool ImageDownloader::isRunning() const
{
	return m_reply != nullptr && m_reply->isRunning();
}

void ImageDownloader::setSize(Image::Size size)
{
	if (size == Image::Size::Unknown) {
		const bool getOriginals = m_profile->getSettings()->value("Save/downloadoriginals", true).toBool();
		const bool hasSample = m_image->url(Image::Size::Sample).isEmpty();
		if (getOriginals || !hasSample) {
			m_size = Image::Size::Full;
		} else {
			m_size = Image::Size::Sample;
		}
	} else {
		m_size = size;
	}
}

void ImageDownloader::setBlacklist(Blacklist *blacklist)
{
	m_blacklist = blacklist;
}

void ImageDownloader::save()
{
	// Always load details if the API doesn't provide the file URL in the listing page
	const QStringList forcedTokens = m_image->parentSite()->getApis().first()->forcedTokens();
	const bool needFileUrl = forcedTokens.contains("*") || forcedTokens.contains("file_url");

	// If we use direct saving or don't want to load tags, we directly save the image
	const int globalNeedTags = needExactTags(m_profile->getSettings());
	const int localNeedTags = m_filename.needExactTags(m_image->parentSite(), m_profile->getSettings());
	const int needTags = qMax(globalNeedTags, localNeedTags);
	const bool filenameNeedTags = needTags == 2 || (needTags == 1 && m_image->hasUnknownTag());
	const bool blacklistNeedTags = m_blacklist != nullptr && m_image->tags().isEmpty();
	if (!blacklistNeedTags && !needFileUrl && (!m_loadTags || !m_paths.isEmpty() || !filenameNeedTags)) {
		loadedSave(Image::LoadTagsResult::Ok);
		return;
	}

	log(QStringLiteral("Not enough information to directly load the image (from blacklist: %1 / from file url: %2 / from filename tags: %3/%4)").arg(blacklistNeedTags).arg(needFileUrl).arg(filenameNeedTags).arg(needTags), Logger::Info);
	connect(m_image.data(), &Image::finishedLoadingTags, this, &ImageDownloader::loadedSave);
	m_image->loadDetails();
}

int ImageDownloader::needExactTags(QSettings *settings) const
{
	int need = 0;

	const auto logFiles = getExternalLogFiles(settings);
	for (auto it = logFiles.constBegin(); it != logFiles.constEnd(); ++it) {
		need = qMax(need, Filename(it.value().value("content").toString()).needExactTags(nullptr, settings));
		if (need == 2) {
			return need;
		}
	}

	QStringList settingNames = QStringList()
		<< "Exec/tag_before"
		<< "Exec/image"
		<< "Exec/tag_after"
		<< "Exec/SQL/before"
		<< "Exec/SQL/tag_before"
		<< "Exec/SQL/image"
		<< "Exec/SQL/tag_after"
		<< "Exec/SQL/after";
	for (const QString &setting : settingNames) {
		const QString value = settings->value(setting, "").toString();
		if (value.isEmpty()) {
			continue;
		}

		need = qMax(need, Filename(value).needExactTags(nullptr, settings));
		if (need == 2) {
			return need;
		}
	}

	return need;
}

void ImageDownloader::abort()
{
	m_image->abortTags();
	if (m_reply != nullptr && m_reply->isRunning()) {
		m_reply->abort();
	}
}

void ImageDownloader::loadedSave(Image::LoadTagsResult result)
{
	disconnect(m_image.data(), &Image::finishedLoadingTags, this, &ImageDownloader::loadedSave);

	// Detect error when loading an image's tags
	if (result != Image::LoadTagsResult::Ok) {
		emit saved(m_image, makeResult({ "" }, Image::SaveResult::DetailsLoadError));
		return;
	}

	// Get the download path from the image if possible
	if (m_paths.isEmpty()) {
		m_paths = m_image->paths(m_filename, m_path, m_count);

		// If we still don't have any paths, that means the filename is invalid
		if (m_paths.isEmpty()) {
			log(QStringLiteral("No path could be generated for filename: '%1'").arg(m_filename.format()), Logger::Error);
			emit saved(m_image, makeResult({ "" }, Image::SaveResult::Error));
			return;
		}

		// Use a random temporary file if we need the MD5 or equivalent
		if (m_filename.needTemporaryFile(m_image->tokens(m_profile))) {
			const QString tmpDir = !m_path.isEmpty() ? m_path : m_profile->tempPath();
			m_temporaryPath = tmpDir + QDir::separator() + QUuid::createUuid().toString().mid(1, 36) + ".tmp";
		}
	}

	// Directly use the image path as temporary file  if possible
	if (m_temporaryPath.isEmpty()) {
		m_temporaryPath = m_paths.first() + ".tmp";
	}

	// Check if the image is blacklisted
	if (m_blacklist != nullptr) {
		const QStringList &detected = m_blacklist->match(m_image->tokens(m_profile));
		if (!detected.isEmpty()) {
			log(QStringLiteral("Image contains blacklisted tags: '%1'").arg(detected.join("', '")), Logger::Info);
			emit saved(m_image, makeResult(m_paths, Image::SaveResult::Blacklisted));
			return;
		}
	}

	// Try to save the image if it's already loaded or exists somewhere else on disk
	if (!m_force) {
		// Check if the destination files already exist
		bool allExists = true;
		for (const QString &path : qAsConst(m_paths)) {
			if (!QFile::exists(path)) {
				allExists = false;
				break;
			}
		}
		if (allExists) {
			log(QStringLiteral("File already exists: `%1`").arg(m_paths.first()), Logger::Info);
			if (m_addMd5) {
				for (const QString &path : qAsConst(m_paths)) {
					addMd5(m_profile, path);
				}
			}
			emit saved(m_image, makeResult(m_paths, Image::SaveResult::AlreadyExistsDisk));
			return;
		}

		// If we don't need any loading, we can return already
		Image::SaveResult res = m_image->preSave(m_temporaryPath, m_size);
		if (res != Image::SaveResult::NotLoaded && (res != Image::SaveResult::AlreadyExistsDeletedMd5 || !m_forceExisting)) {
			QList<ImageSaveResult> result {{ m_temporaryPath, m_size, res }};

			if (res == Image::SaveResult::Saved || res == Image::SaveResult::Copied || res == Image::SaveResult::Moved || res == Image::SaveResult::Shortcut || res == Image::SaveResult::Linked) {
				result = postSaving(res);
			}

			emit saved(m_image, result);
			return;
		}
	}

	m_url = m_image->url(m_size);

	if (m_url.isEmpty()) {
		log(QStringLiteral("Image without URL found for '%1'").arg(m_paths.first()), Logger::Warning);
		emit saved(m_image, makeResult(m_paths, Image::SaveResult::NetworkError));
		return;
	}

	log(QStringLiteral("Loading and saving image from `%1` in `%2`").arg(m_url.toString(), m_paths.first()));
	loadImage();
}

void ImageDownloader::loadImage()
{
	connect(&m_fileDownloader, &FileDownloader::success, this, &ImageDownloader::success, Qt::UniqueConnection);
	connect(&m_fileDownloader, &FileDownloader::networkError, this, &ImageDownloader::networkError, Qt::UniqueConnection);
	connect(&m_fileDownloader, &FileDownloader::writeError, this, &ImageDownloader::writeError, Qt::UniqueConnection);

	// Delete previous replies for retries
	if (m_reply != nullptr) {
		m_reply->deleteLater();
	}

	// Load the image directly on the disk
	Site *site = m_image->parentSite();
	m_reply = site->get(site->fixUrl(m_url.toString()), Site::QueryType::Img, m_image->parentUrl(), QStringLiteral("image"), m_image.data());
	m_reply->setParent(this);
	connect(m_reply, &NetworkReply::downloadProgress, this, &ImageDownloader::downloadProgressImage);

	// Create download root directory
	const QString rootDir = m_temporaryPath.section(QDir::separator(), 0, -2);
	if (!QDir(rootDir).exists() && !QDir().mkpath(rootDir)) {
		log(QStringLiteral("Impossible to create the destination folder: %1.").arg(rootDir), Logger::Error);
		emit saved(m_image, makeResult(m_paths, Image::SaveResult::Error));
		return;
	}

	// If we can't start writing for some reason, return an error
	if (!m_fileDownloader.start(m_reply, m_temporaryPath)) {
		emit saved(m_image, makeResult(m_paths, Image::SaveResult::Error));
		return;
	}
}

void ImageDownloader::downloadProgressImage(qint64 v1, qint64 v2)
{
	if (m_image->fileSize() == 0 || m_image->fileSize() < v2 / 2) {
		m_image->setFileSize(v2, currentSize());
	}

	emit downloadProgress(m_image, v1, v2);
}

Image::Size ImageDownloader::currentSize() const
{
	return m_tryingSample ? Image::Size::Sample : m_size;
}

QList<ImageSaveResult> ImageDownloader::makeResult(const QStringList &paths, Image::SaveResult result) const
{
	const Image::Size size = currentSize();

	QList<ImageSaveResult> res;
	for (const QString &path : paths) {
		res.append({ path, size, result });
	}
	return res;
}

void ImageDownloader::writeError()
{
	emit saved(m_image, makeResult(m_paths, Image::SaveResult::Error));
}

void ImageDownloader::networkError(NetworkReply::NetworkError error, const QString &msg)
{
	if (error == NetworkReply::NetworkError::ContentNotFoundError) {
		QSettings *settings = m_profile->getSettings();
		ExtensionRotator *extensionRotator = m_image->extensionRotator();

		const bool sampleFallback = settings->value("Save/samplefallback", true).toBool();
		const bool shouldFallback = m_size == Image::Size::Full && sampleFallback && !m_image->url(Image::Size::Sample).isEmpty();
		QString newext = extensionRotator != nullptr ? extensionRotator->next() : QString();

		if (m_rotate && !newext.isEmpty()) {
			m_url = setExtension(m_image->url(m_size), newext);
			log(QStringLiteral("Image not found. New try with extension %1 (%2)...").arg(newext, m_url.toString()));
			m_image->setUrl(m_url);
			loadImage();
		} else if (shouldFallback && !m_tryingSample) {
			m_url = m_image->url(Image::Size::Sample);
			m_tryingSample = true;
			log(QStringLiteral("Image not found. New try with its sample (%1)...").arg(m_url.toString()), Logger::Warning);
			m_image->setUrl(m_url);
			loadImage();
		} else {
			m_tryingSample = false;
			log(QStringLiteral("Image not found."));
			emit saved(m_image, makeResult(m_paths, Image::SaveResult::NotFound));
		}
	} else if (error != NetworkReply::NetworkError::OperationCanceledError) {
		// "HostNotFoundError" might be caused by network loss, so we emit a blocking "Error" instead
		if (error == NetworkReply::NetworkError::HostNotFoundError || msg.contains("unreachable")) {
			log(QStringLiteral("Host '%1' not found for the image: `%2`: %3 (%4)").arg(m_reply->url().host(), m_image->url().toString().toHtmlEscaped()).arg(error).arg(msg), Logger::Error);
			emit saved(m_image, makeResult(m_paths, Image::SaveResult::Error));
			return;
		}

		log(QStringLiteral("Network error for the image: `%1`: %2 (%3)").arg(m_image->url().toString().toHtmlEscaped()).arg(error).arg(msg), Logger::Error);
		emit saved(m_image, makeResult(m_paths, Image::SaveResult::NetworkError));
	}
}

void ImageDownloader::success()
{
	// Handle network redirects
	QUrl redir = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty()) {
		m_url = redir;
		loadImage();
		return;
	}

	emit saved(m_image, postSaving());
}

QList<ImageSaveResult> ImageDownloader::postSaving(Image::SaveResult saveResult)
{
	QSettings *settings = m_profile->getSettings();

	const QString multipleFiles = settings->value("Save/multiple_files", "copy").toString();
	const Image::Size size = currentSize();

	m_image->setSavePath(m_temporaryPath, size);

	// Load size if necessary
	if (m_image->size(size).isEmpty()) {
		QImageReader reader(m_temporaryPath);
		QSize imgSize = reader.size();
		if (imgSize.isValid()) {
			m_image->setSize(imgSize, size);
		}
	}

	// Resize image if necessary
	bool maxWidthEnabled = settings->value("ImageSize/maxWidthEnabled", false).toBool();
	bool maxHeightEnabled = settings->value("ImageSize/maxHeightEnabled", false).toBool();
	QSize resizeBox = m_image->size(size);
	if (!resizeBox.isEmpty() && (maxWidthEnabled || maxHeightEnabled)) {
		int maxWidth = settings->value("ImageSize/maxWidth", 1000).toInt();
		if (maxWidthEnabled && resizeBox.width() > maxWidth) {
			resizeBox.setWidth(maxWidth);
		}
		int maxHeight = settings->value("ImageSize/maxHeight", 1000).toInt();
		if (maxHeightEnabled && resizeBox.height() > maxHeight) {
			resizeBox.setWidth(maxHeight);
		}
		if (resizeBox != m_image->size(size)) {
			QImage img(m_temporaryPath);
			img = img.scaled(resizeBox, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			img.save(m_temporaryPath, m_image->extension().toStdString().c_str());
		}
	}

	if (!m_filename.format().isEmpty()) {
		m_paths = m_image->paths(m_filename, m_path, m_count);
	}

	QString suffix;
	#ifdef Q_OS_WIN
		if (saveResult == Image::SaveResult::Shortcut) {
			suffix = ".lnk";
		}
	#endif

	QFile tmp(m_temporaryPath + suffix);
	bool moved = false;

	QList<ImageSaveResult> result;
	for (const QString &file : qAsConst(m_paths)) {
		const QString path = file + suffix;

		// Don't overwrite already existing files
		if (QFile::exists(file) || (!suffix.isEmpty() && QFile::exists(path))) {
			log(QStringLiteral("File already exists: `%1`").arg(file), Logger::Info);
			if (suffix.isEmpty() && m_addMd5) {
				addMd5(m_profile, file);
			}
			result.append({ path, size, Image::SaveResult::AlreadyExistsDisk });
			continue;
		}

		const QString dir = path.section(QDir::separator(), 0, -2);
		if (!QDir(dir).exists() && !QDir().mkpath(dir)) {
			log(QStringLiteral("Impossible to create the destination folder: %1.").arg(dir), Logger::Error);
			result.append({ path, size, Image::SaveResult::Error });
			continue;
		}

		if (!moved) {
			if (!tmp.rename(path)) {
				log(QStringLiteral("Error renaming from `%1` to `%2`").arg(tmp.fileName(), path), Logger::Error);
				result.append({ path, size, Image::SaveResult::Error });
				continue;
			} else {
				moved = true;
			}
		} else if (multipleFiles == "link") {
			#ifdef Q_OS_WIN
				bool ok = tmp.link(path + ".lnk");
			#else
				bool ok = tmp.link(path);
			#endif
			if (!ok) {
				log(QStringLiteral("Error creating link from `%1` to `%2`").arg(tmp.fileName(), path), Logger::Error);
				result.append({ path, size, Image::SaveResult::Error });
				continue;
			}
		} else {
			if (!tmp.copy(path)) {
				log(QStringLiteral("Error copying from `%1` to `%2`").arg(tmp.fileName(), path), Logger::Error);
				result.append({ path, size, Image::SaveResult::Error });
				continue;
			}
		}

		result.append({ path, size, saveResult });
		if (m_postSave) {
			m_image->postSave(path, size, saveResult, m_addMd5, m_startCommands, m_count);
		}
	}

	if (!moved) {
		tmp.remove();
	}

	return result;
}
