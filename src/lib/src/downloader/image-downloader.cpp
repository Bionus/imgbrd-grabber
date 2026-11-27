#include "downloader/image-downloader.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QSettings>
#include <QSize>
#include <QUuid>
#include <algorithm>
#include <utility>
#include "extension-rotator.h"
#include "file-downloader.h"
#include "filename/filename.h"
#include "functions.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/image.h"
#include "models/metadata-database/metadata-database.h"
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


/**
 * Register existing destination files that may be missing from the MD5 database.
 * Trust paths already registered for the source MD5, otherwise hash the existing file,
 * since a filename collision does not mean it contains the requested image.
 */
static void registerExistingFiles(Profile *profile, const QString &md5, const QStringList &paths)
{
	QStringList registered = profile->md5Exists(md5);
	std::transform(registered.begin(), registered.end(), registered.begin(), QDir::toNativeSeparators);
	for (const QString &path : paths) {
		if (!registered.contains(QDir::toNativeSeparators(path))) {
			addMd5(profile, path);
		}
	}
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
		const bool hasSample = !m_image->url(Image::Size::Sample).isEmpty();
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
	// Try to build a path first if we don't need tags for the filename, to check for the "same dir" MD5 setting
	const int filenameTagLevel = m_filename.needExactTags(m_image->parentSite(), m_profile->getSettings());
	const bool filenameNeedExactTags = filenameTagLevel == 2 || (filenameTagLevel == 1 && m_image->hasUnknownTag());
	const QStringList paths = m_image->paths(m_filename, m_path, m_count);
	const QString md5Path = !filenameNeedExactTags && !paths.isEmpty() ? paths.first() : QString();

	// Whether we might update the metadata of files that already exist, in which case we can't skip loading their details
	const bool updateMetadata = shouldUpdateMetadata();
	const auto fileExists = [](const QString &path) { return QFile::exists(path); };
	const QStringList &destinations = !m_paths.isEmpty() ? m_paths : paths;
	const bool destinationsExist = !destinations.isEmpty() && std::all_of(destinations.constBegin(), destinations.constEnd(), fileExists);

	// Existing destinations do not run post-save commands or write metadata, so they do not need details.
	const bool blacklistNeedTags = m_blacklist != nullptr && !m_blacklist->isEmpty() && m_image->tags().isEmpty();
	if (!m_force && !updateMetadata && !blacklistNeedTags && !filenameNeedExactTags && !m_filename.needTemporaryFile(m_image->tokens(m_profile)) && !paths.isEmpty()
		&& std::all_of(paths.constBegin(), paths.constEnd(), fileExists)) {
		loadedSave(Image::LoadTagsResult::Ok);
		return;
	}

	// We don't need to load the image details of files already in the MD5 list and that should be skipped
	const QPair<QString, QString> md5Result = m_profile->md5Action(m_image->md5(), md5Path);
	const QString &md5action = md5Result.first;
	const bool md5Exists = md5action == "ignore" && !md5Result.second.isEmpty() && QFile::exists(md5Result.second);
	if (md5action == "ignore" && !m_force && !(updateMetadata && md5Exists)) {
		loadedSave(Image::LoadTagsResult::Ok);
		return;
	}

	// Existing files whose metadata will be updated don't need to be downloaded again
	const bool existingToUpdate = !m_force && updateMetadata && (destinationsExist || md5Exists);

	// A local MD5 duplicate does not need a file URL, but may still need details for naming or post-save operations.
	const bool reuseFile = !m_force && m_size != Image::Size::Thumbnail && !md5Path.isEmpty()
		&& !m_filename.needTemporaryFile(m_image->tokens(m_profile))
		&& (md5action == "copy" || md5action == "move" || md5action == "link" || md5action == "hardlink");
	const QStringList forcedTokens = m_image->parentSite()->getApis().first()->forcedTokens();
	const bool needFileUrl = !reuseFile && !existingToUpdate && (forcedTokens.contains("*") || forcedTokens.contains("file_url"));
	const bool needUgoiraData = !existingToUpdate && m_image->extension() == QStringLiteral("zip") && m_profile->getSettings()->value("Save/ConvertUgoira", false).toBool();

	// If we use direct saving or don't want to load tags, we directly save the image
	const int globalNeedTags = needExactTags(m_profile->getSettings());
	const int localNeedTags = m_filename.needExactTags(m_image->parentSite(), m_profile->getSettings());
	const int needTags = qMax(globalNeedTags, localNeedTags);
	const bool filenameNeedTags = needTags == 2 || (needTags == 1 && m_image->hasUnknownTag());
	const int metadataTagLevel = existingToUpdate ? metadataNeedExactTags(m_profile->getSettings()) : 0;
	const bool metadataNeedTags = metadataTagLevel == 2 || (metadataTagLevel == 1 && m_image->hasUnknownTag());
	if (!blacklistNeedTags && !needFileUrl && !needUgoiraData && !metadataNeedTags && (!m_loadTags || !m_paths.isEmpty() || !filenameNeedTags)) {
		loadedSave(Image::LoadTagsResult::Ok);
		return;
	}

	log(QStringLiteral("Not enough information to directly load the image (from blacklist: %1 / from file url: %2 / from filename tags: %3/%4 / from metadata update: %5)").arg(blacklistNeedTags).arg(needFileUrl).arg(filenameNeedTags).arg(needTags).arg(metadataNeedTags), Logger::Info);
	connect(m_image.data(), &Image::finishedLoadingTags, this, &ImageDownloader::loadedSave);
	m_image->loadDetails();
}

int ImageDownloader::needExactTags(QSettings *settings) const
{
	int need = 0;

	// Check external log files
	const auto logFiles = getExternalLogFiles(settings);
	for (auto it = logFiles.constBegin(); it != logFiles.constEnd(); ++it) {
		need = qMax(need, Filename(it.value().value("content").toString()).needExactTags(nullptr, settings));
		if (need == 2) {
			return need;
		}
	}

	// Check commands
	static const QStringList settingNames {
		"Exec/tag_before",
		"Exec/image",
		"Exec/tag_after",
		"Exec/SQL/before",
		"Exec/SQL/tag_before",
		"Exec/SQL/image",
		"Exec/SQL/tag_after",
		"Exec/SQL/after",
	};
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

	// Check metadata
	return qMax(need, metadataNeedExactTags(settings));
}

int ImageDownloader::metadataNeedExactTags(QSettings *settings) const
{
	int need = 0;

	// Check Exiftool metadata
	for (const auto &pair : getMetadataExiftool(settings)) {
		need = qMax(need, Filename(pair.second).needExactTags(nullptr, settings));
		if (need == 2) {
			return need;
		}
	}

	#ifdef WIN_FILE_PROPS
		// Check Windows Property System
		for (const auto &pair : getMetadataPropsys(settings)) {
			need = qMax(need, Filename(pair.second).needExactTags(nullptr, settings));
			if (need == 2) {
				return need;
			}
		}
	#endif

	return need;
}

/**
 * Check whether the metadata of already existing files for this image should be updated.
 *
 * @param paths The existing files, used to get a date if the image does not have one.
 */
bool ImageDownloader::shouldUpdateMetadata(const QStringList &paths) const
{
	if (m_force || !m_postSave || m_size == Image::Size::Thumbnail) {
		return false;
	}

	QSettings *settings = m_profile->getSettings();
	if (!settings->value("Save/MetadataUpdateExisting", false).toBool()) {
		return false;
	}

	// Only update once per website, using the metadata database to know which posts were already fetched
	if (settings->value("Save/MetadataUpdateOncePerSource", false).toBool() && settings->value("Save/MetadataDatabase", false).toBool() && m_image->id() > 0) {
		MetadataDatabase *database = m_profile->metadataDatabase();
		if (database != nullptr && database->exists(m_image->parentSite()->url(), QString::number(m_image->id()))) {
			return false;
		}
	}

	// No age limit
	const int maxAge = settings->value("Save/MetadataUpdateMaxAge", 0).toInt();
	if (maxAge <= 0) {
		return true;
	}

	// Use the post date, or fallback to the file date if the image doesn't have one
	QDateTime date = m_image->createdAt();
	if (!date.isValid()) {
		// When keeping the original date, the modification date is the post date on all platforms (only Windows can set the creation date)
		// Otherwise, we use the creation date as the modification date is reset every time the metadata is written
		const bool keepDate = settings->value("Save/keepDate", true).toBool();
		for (const QString &path : paths) {
			const QFileInfo info(path);
			if (info.exists()) {
				date = keepDate || !info.birthTime().isValid() ? info.lastModified() : info.birthTime();
				break;
			}
		}
	}

	// If we can't know the age, we only skip loading the details when we have no file to check yet
	if (!date.isValid()) {
		return true;
	}

	return date.secsTo(QDateTime::currentDateTime()) <= static_cast<qint64>(maxAge) * 24 * 60 * 60;
}

/**
 * Update the metadata of the given existing files, if enabled in the settings.
 */
void ImageDownloader::updateExistingMetadata(const QStringList &paths)
{
	if (!shouldUpdateMetadata(paths)) {
		return;
	}

	for (const QString &path : paths) {
		m_image->updateMetadata(path);
	}
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
				registerExistingFiles(m_profile, m_image->md5(), m_paths);
			}
			updateExistingMetadata(m_paths);
			emit saved(m_image, makeResult(m_paths, Image::SaveResult::AlreadyExistsDisk));
			return;
		}

		// If we don't need any loading, we can return already
		Image::SaveResult res = m_image->preSave(m_temporaryPath, m_size);
		if (res == Image::SaveResult::AlreadyExistsMd5 && shouldUpdateMetadata()) {
			updateExistingMetadata(m_profile->md5Exists(m_image->md5()));
		}
		if (res != Image::SaveResult::NotLoaded && (res != Image::SaveResult::AlreadyExistsDeletedMd5 || !m_forceExisting)) {
			QList<ImageSaveResult> preResult {{ m_temporaryPath, m_size, res }}; // TODO(Bionus): this should use the MD5 path if possible

			if (res == Image::SaveResult::Saved || res == Image::SaveResult::Copied || res == Image::SaveResult::Moved || res == Image::SaveResult::Shortcut || res == Image::SaveResult::Linked) {
				preResult = afterTemporarySave(res);
			}

			emit saved(m_image, preResult);
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

void ImageDownloader::loadImage(bool rateLimit)
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
	m_reply = site->get(site->fixUrl(m_url.toString()), rateLimit ? Site::QueryType::Retry : Site::QueryType::Img, m_image->parentUrl(), QStringLiteral("image"), m_image.data());
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
		const QString newExt = extensionRotator != nullptr ? extensionRotator->next() : QString();

		if (m_rotate && !newExt.isEmpty()) {
			m_url = setExtension(m_image->url(m_size), newExt);
			log(QStringLiteral("Image not found. New try with extension %1 (%2)...").arg(newExt, m_url.toString()));
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
			log(QStringLiteral("Host '%1' not found for the image: `%2`: %3 (%4)").arg(m_reply->url().host(), m_image->url().toString()).arg(error).arg(msg), Logger::Error);
			emit saved(m_image, makeResult(m_paths, Image::SaveResult::Error));
			return;
		}

		// Detect HTTP 429 / 503 / 509 usage limit reached
		const int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (statusCode == 429 || statusCode == 503 || statusCode == 509) {
			log(QStringLiteral("Rate limit reached (%1) for the image `%2`. New try.").arg(QString::number(statusCode), m_image->url().toString()), Logger::Warning);
			loadImage(true);
			return;
		}

		log(QStringLiteral("Network error for the image: `%1`: %2 (%3)").arg(m_image->url().toString()).arg(error).arg(msg), Logger::Error);
		emit saved(m_image, makeResult(m_paths, Image::SaveResult::NetworkError));
	}
}

void ImageDownloader::success()
{
	// Handle network redirects
	const QUrl redirect = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirect.isEmpty()) {
		// Detect Cloudflare redirects to error or home pages
		if (m_reply->rawHeader("server") == "cloudflare" && getExtension(m_url) != getExtension(redirect)) {
			log(QStringLiteral("Cloudflare redirect image `%1` to `%2` ignored").arg(m_url.toString(), redirect.toString()), Logger::Info);
			QFile::remove(m_temporaryPath);
			emit saved(m_image, makeResult(m_paths, Image::SaveResult::NetworkError));
			return;
		}

		log(QStringLiteral("Redirecting image `%1` to `%2`").arg(m_url.toString(), redirect.toString()), Logger::Info);
		m_url = redirect;
		loadImage();
		return;
	}

	emit saved(m_image, afterTemporarySave(Image::SaveResult::Saved));
}

QList<ImageSaveResult> ImageDownloader::afterTemporarySave(Image::SaveResult saveResult)
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
			resizeBox.setHeight(maxHeight);
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
	QStringList existingPaths;

	QList<ImageSaveResult> result;
	for (const QString &file : qAsConst(m_paths)) {
		QString path = file + suffix;

		// Don't overwrite already existing files
		if (QFile::exists(file) || (!suffix.isEmpty() && QFile::exists(path))) {
			log(QStringLiteral("File already exists: `%1`").arg(file), Logger::Info);
			if (suffix.isEmpty() && m_addMd5) {
				existingPaths.append(file);
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

		if (m_postSave) {
			path = m_image->postSave(path, size, saveResult, m_addMd5, m_startCommands, m_count);
		}
		result.append({ path, size, saveResult });
	}

	if (!existingPaths.isEmpty()) {
		registerExistingFiles(m_profile, m_image->md5(), existingPaths);
	}

	if (!moved) {
		tmp.remove();
	}

	return result;
}
