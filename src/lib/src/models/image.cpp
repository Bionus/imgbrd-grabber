#include <QCryptographicHash>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QSettings>
#include <QTimer>
#include <utility>
#include "commands/commands.h"
#include "downloader/extension-rotator.h"
#include "favorite.h"
#include "functions.h"
#include "loader/token.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/filename.h"
#include "models/image.h"
#include "models/page.h"
#include "models/pool.h"
#include "models/profile.h"
#include "models/site.h"
#include "network/network-reply.h"
#include "tags/tag.h"
#include "tags/tag-database.h"
#include "tags/tag-stylist.h"
#include "tags/tag-type.h"

#define MAX_LOAD_FILESIZE (1024 * 1024 * 50)


Image::Image()
	: m_profile(nullptr), m_extensionRotator(nullptr)
{}

// TODO(Bionus): clean up this mess
Image::Image(const Image &other)
	: QObject(other.parent())
{
	m_parent = other.m_parent;
	m_parentUrl = other.m_parentUrl;
	m_isGallery = other.m_isGallery;

	m_id = other.m_id;

	m_url = other.m_url;
	m_md5 = other.m_md5;
	m_name = other.m_name;
	m_sources = other.m_sources;

	m_pageUrl = other.m_pageUrl;

	m_sizes = other.m_sizes;
	m_data = other.m_data;

	m_galleryCount = other.m_galleryCount;
	m_position = other.m_position;

	m_loadDetails = other.m_loadDetails;

	m_tags = other.m_tags;
	m_pools = other.m_pools;
	m_profile = other.m_profile;
	m_settings = other.m_settings;
	m_search = other.m_search;
	m_parentSite = other.m_parentSite;

	m_extensionRotator = other.m_extensionRotator;
	m_loadingDetails = other.m_loadingDetails;
}

Image::Image(Profile *profile)
	: m_profile(profile), m_settings(profile->getSettings())
{}

Image::Image(Site *site, QMap<QString, QString> details, Profile *profile, Page *parent)
	: Image(site, details, QVariantMap(), profile, parent)
{}

Image::Image(Site *site, QMap<QString, QString> details, QVariantMap data, Profile *profile, Page *parent)
	: m_profile(profile), m_parent(parent), m_id(0), m_parentSite(site), m_extensionRotator(nullptr), m_data(std::move(data))
{
	m_settings = m_profile->getSettings();

	// Parents
	if (m_parentSite == nullptr) {
		log(QStringLiteral("Image has nullptr parent, aborting creation."));
		return;
	}
	if (m_parent != nullptr) {
		m_parentUrl = m_parent->url();
	}

	// Other details
	m_isGallery = details.contains("type") && details["type"] == "gallery";
	m_md5 = details.contains("md5") ? details["md5"] : "";
	m_name = details.contains("name") ? details["name"] : "";
	m_search = parent != nullptr ? parent->search() : (details.contains("search") ? details["search"].split(' ') : QStringList());
	m_id = details.contains("id") ? details["id"].toULongLong() : 0;
	m_sources = details.contains("sources") ? details["sources"].split('\n') : (details.contains("source") ? QStringList { details["source"] } : QStringList());
	m_galleryCount = details.contains("gallery_count") ? details["gallery_count"].toInt() : -1;
	m_position = details.contains("position") ? details["position"].toInt() : 0;

	// Sizes
	static const QMap<Image::Size, QString> prefixes
	{
		{ Image::Size::Full, "" },
		{ Image::Size::Sample, "sample_" },
		{ Image::Size::Thumbnail, "preview_" },
	};
	for (auto it = prefixes.constBegin(); it != prefixes.constEnd(); ++it) {
		const QString &prefix = it.value();

		auto is = QSharedPointer<ImageSize>::create();

		const QString &urlKey = (prefix.isEmpty() ? "file_" : prefix) + "url";
		is->url = details.contains(urlKey) ? removeCacheBuster(m_parentSite->fixUrl(details[urlKey])) : QString();

		is->size = details.contains(prefix + "width") && details.contains(prefix + "height")
			? QSize(details[prefix + "width"].toInt(), details[prefix + "height"].toInt())
			: QSize();
		is->fileSize = details.contains(prefix + "file_size") ? details[prefix + "file_size"].toInt() : 0;

		if (details.contains(prefix + "rect")) {
			const QStringList rect = details[prefix + "rect"].split(';');
			if (rect.count() == 4) {
				is->rect = QRect(rect[0].toInt(), rect[1].toInt(), rect[2].toInt(), rect[3].toInt());
			} else {
				log("Invalid number of values for image rectangle", Logger::Error);
			}
		}

		m_sizes.insert(it.key(), is);
	}

	// Page url
	if (details.contains("page_url")) {
		m_pageUrl = details["page_url"];
	}

	// Tags
	if (m_data.contains("tags")) {
		m_tags = m_data["tags"].value<QList<Tag>>();
		m_data.remove("tags");
	}

	// Complete missing tag type information
	m_parentSite->tagDatabase()->load();
	QStringList unknownTags;
	for (const Tag &tag : qAsConst(m_tags)) {
		if (tag.type().isUnknown()) {
			unknownTags.append(tag.text());
		}
	}
	QMap<QString, TagType> dbTypes = m_parentSite->tagDatabase()->getTagTypes(unknownTags);
	for (Tag &tag : m_tags) {
		if (dbTypes.contains(tag.text())) {
			tag.setType(dbTypes[tag.text()]);
		}
	}

	// Get file url and try to improve it to save bandwidth
	m_url = m_sizes[Size::Full]->url;
	const QString ext = getExtension(m_url);
	if (details.contains("ext") && !details["ext"].isEmpty()) {
		const QString realExt = details["ext"];
		if (ext != realExt) {
			setFileExtension(realExt);
		}
	} else if (ext == QLatin1String("jpg") && !url(Size::Thumbnail).isEmpty()) {
		bool fixed = false;
		const QString previewExt = getExtension(url(Size::Thumbnail));
		if (!url(Size::Sample).isEmpty()) {
			// Guess extension from sample url
			const QString sampleExt = getExtension(url(Size::Sample));
			if (sampleExt != QLatin1String("jpg") && sampleExt != QLatin1String("png") && sampleExt != ext && previewExt == ext) {
				m_url = setExtension(m_url, sampleExt);
				fixed = true;
			}
		}

		// Guess the extension from the tags
		if (!fixed) {
			if ((hasTag(QStringLiteral("swf")) || hasTag(QStringLiteral("flash"))) && ext != QLatin1String("swf")) {
				setFileExtension(QStringLiteral("swf"));
			} else if ((hasTag(QStringLiteral("gif")) || hasTag(QStringLiteral("animated_gif"))) && ext != QLatin1String("webm") && ext != QLatin1String("mp4")) {
				setFileExtension(QStringLiteral("gif"));
			} else if (hasTag(QStringLiteral("mp4")) && ext != QLatin1String("gif") && ext != QLatin1String("webm")) {
				setFileExtension(QStringLiteral("mp4"));
			} else if (hasTag(QStringLiteral("animated_png")) && ext != QLatin1String("webm") && ext != QLatin1String("mp4")) {
				setFileExtension(QStringLiteral("png"));
			} else if ((hasTag(QStringLiteral("webm")) || hasTag(QStringLiteral("animated"))) && ext != QLatin1String("gif") && ext != QLatin1String("mp4")) {
				setFileExtension(QStringLiteral("webm"));
			}
		}
	} else if (details.contains("image") && details["image"].contains("MB // gif\" height=\"") && ext != QLatin1String("gif")) {
		m_url = setExtension(m_url, QStringLiteral("gif"));
	} else if (ext == QLatin1String("webm") && hasTag(QStringLiteral("mp4"))) {
		m_url = setExtension(m_url, QStringLiteral("mp4"));
	}

	// Remove ? in urls
	m_url = removeCacheBuster(m_url);

	// We use the sample URL as the URL for zip files (ugoira) or if the setting is set
	const bool downloadOriginals = m_settings->value("Save/downloadoriginals", true).toBool();
	if (!url(Size::Sample).isEmpty() && (getExtension(m_url) == "zip" || !downloadOriginals)) {
		m_url = url(Size::Sample).toString();
	}

	init();
}

void Image::init()
{
	// Page URL
	if (m_pageUrl.isEmpty()) {
		Api *api = m_parentSite->detailsApi();
		if (api != nullptr) {
			m_pageUrl = api->detailsUrl(m_id, m_md5, m_parentSite).url;
		}
	}
	m_pageUrl = m_parentSite->fixUrl(m_pageUrl).toString();

	// Setup extension rotator
	const bool animated = hasTag("gif") || hasTag("animated_gif") || hasTag("mp4") || hasTag("animated_png") || hasTag("webm") || hasTag("animated");
	const QStringList extensions = animated
		? QStringList { "webm", "mp4", "gif", "jpg", "png", "jpeg", "swf" }
		: QStringList { "jpg", "png", "gif", "jpeg", "webm", "swf", "mp4" };
	m_extensionRotator = new ExtensionRotator(getExtension(m_url), extensions, this);
}


static const QMap<Image::Size, QString> sizeToStringMap
{
	{ Image::Size::Full, "full" },
	{ Image::Size::Sample, "sample" },
	{ Image::Size::Thumbnail, "thumbnail" },
};

void Image::write(QJsonObject &json) const
{
	json["website"] = m_parentSite->url();

	// Parent gallery
	if (!m_parentGallery.isNull()) {
		QJsonObject jsonGallery;
		m_parentGallery->write(jsonGallery);
		json["gallery"] = jsonGallery;
	}

	// Sizes
	QJsonObject jsonSizes;
	for (const auto &size : m_sizes.keys()) {
		QJsonObject jsonSize;
		m_sizes[size]->write(jsonSize);
		if (!jsonSize.isEmpty() && sizeToStringMap.contains(size)) {
			jsonSizes[sizeToStringMap[size]] = jsonSize;
		}
	}
	if (!jsonSizes.isEmpty()) {
		json["sizes"] = jsonSizes;
	}

	// Tags
	QStringList tags;
	tags.reserve(m_tags.count());
	for (const Tag &tag : m_tags) {
		tags.append(tag.text());
	}

	// FIXME: real serialization
	json["name"] = m_name;
	json["id"] = QString::number(m_id);
	json["md5"] = m_md5;
	json["tags"] = QJsonArray::fromStringList(tags);
	json["url"] = m_url.toString();
	json["search"] = QJsonArray::fromStringList(m_search);

	// Arbitrary tokens
	QJsonObject jsonData;
	for (const auto &key : m_data.keys()) {
		jsonData[key] = (QMetaType::Type) m_data[key].type() == QMetaType::QDateTime
			? "date:" + m_data[key].toDateTime().toString(Qt::ISODate)
			: QJsonValue::fromVariant(m_data[key]);
	}
	if (!jsonData.isEmpty()) {
		json["data"] = jsonData;
	}
}

bool Image::read(const QJsonObject &json, const QMap<QString, Site*> &sites)
{
	const QString site = json["website"].toString();
	if (!sites.contains(site)) {
		return false;
	}

	if (json.contains("gallery")) {
		auto gallery = new Image(m_profile);
		if (gallery->read(json["gallery"].toObject(), sites)) {
			m_parentGallery = QSharedPointer<Image>(gallery);
		} else {
			gallery->deleteLater();
			return false;
		}
	}

	m_parentSite = sites[site];

	// Sizes
	for (const auto &size : sizeToStringMap.keys()) {
		auto sizeObj = QSharedPointer<ImageSize>::create();
		const QString &key = sizeToStringMap[size];
		if (json.contains("sizes")) {
			const auto &jsonSizes = json["sizes"].toObject();
			if (jsonSizes.contains(key)) {
				sizeObj->read(jsonSizes[key].toObject());
			}
		}
		m_sizes[size] = sizeObj;
	}

	// Tags
	QJsonArray jsonTags = json["tags"].toArray();
	m_tags.reserve(jsonTags.count());
	for (const auto &tag : jsonTags) {
		m_tags.append(Tag(tag.toString()));
	}

	// Search
	QJsonArray jsonSearch = json["search"].toArray();
	m_search.reserve(jsonSearch.count());
	for (const auto &tag : jsonSearch) {
		m_search.append(tag.toString());
	}

	// Basic fields
	m_name = json["name"].toString();
	m_id = json["id"].toString().toULongLong();
	m_md5 = json["md5"].toString();

	// Arbitrary tokens
	if (json.contains("data")) {
		const auto &jsonData = json["data"].toObject();
		for (const auto &key : jsonData.keys()) {
			QVariant val = jsonData[key].toVariant();
			if (val.toString().startsWith("date:")) {
				val = QDateTime::fromString(val.toString().mid(5), Qt::ISODate);
			}
			m_data[key] = val;
		}
	}

	// URL with fallback
	if (json.contains("file_url")) {
		m_url = json["file_url"].toString();
		if (m_sizes[Size::Full]->url.isEmpty()) {
			m_sizes[Size::Full]->url = m_url;
		}
	} else {
		m_url = json.contains("url") ? json["url"].toString() : m_sizes[Size::Full]->url;
	}

	init();
	return true;
}


void Image::loadDetails(bool rateLimit)
{
	if (m_loadingDetails) {
		return;
	}

	if (m_loadedDetails || m_pageUrl.isEmpty()) {
		emit finishedLoadingTags();
		return;
	}

	if (m_loadDetails != nullptr) {
		if (m_loadDetails->isRunning()) {
			m_loadDetails->abort();
		}

		m_loadDetails->deleteLater();
	}

	Site::QueryType type = rateLimit ? Site::QueryType::Retry : Site::QueryType::List;
	m_loadDetails = m_parentSite->get(m_pageUrl, type);
	m_loadDetails->setParent(this);
	m_loadingDetails = true;

	connect(m_loadDetails, &NetworkReply::finished, this, &Image::parseDetails);
}
void Image::abortTags()
{
	if (m_loadingDetails && m_loadDetails->isRunning()) {
		m_loadDetails->abort();
		m_loadingDetails = false;
	}
}
void Image::parseDetails()
{
	m_loadingDetails = false;

	// Check redirection
	QUrl redir = m_loadDetails->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty()) {
		m_pageUrl = m_parentSite->fixUrl(redir);
		log(QStringLiteral("Redirecting details page to `%1`").arg(m_pageUrl.toString()));
		loadDetails();
		return;
	}

	const int statusCode = m_loadDetails->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 429) {
		log(QStringLiteral("Details limit reached (429). New try."));
		loadDetails(true);
		return;
	}

	// Aborted or connection error
	if (m_loadDetails->error()) {
		if (m_loadDetails->error() != NetworkReply::NetworkError::OperationCanceledError) {
			log(QStringLiteral("Loading error for '%1': %2").arg(m_pageUrl.toString(), m_loadDetails->errorString()), Logger::Error);
		}
		m_loadDetails->deleteLater();
		m_loadDetails = nullptr;
		return;
	}

	const QString source = QString::fromUtf8(m_loadDetails->readAll());

	// Get an api able to parse details
	Api *api = m_parentSite->detailsApi();
	if (api == nullptr) {
		return;
	}

	// Parse source
	ParsedDetails ret = api->parseDetails(source, statusCode, m_parentSite);
	if (!ret.error.isEmpty()) {
		auto logLevel = m_detailsParsWarnAsErr ? Logger::Error : Logger::Warning;
		log(QStringLiteral("[%1][%2] %3").arg(m_parentSite->url(), api->getName(), ret.error), logLevel);
		emit finishedLoadingTags();
		return;
	}

	// Fill data from parsing result
	if (!ret.pools.isEmpty()) {
		m_pools = ret.pools;
	}
	if (!ret.tags.isEmpty()) {
		m_tags = ret.tags;
	}
	if (ret.createdAt.isValid()) {
		m_data["date"] = ret.createdAt;
	}

	// Image url
	if (!ret.imageUrl.isEmpty()) {
		const QUrl before = m_url;
		const QUrl newUrl = m_parentSite->fixUrl(ret.imageUrl, before);

		m_url = newUrl;
		m_sizes[Size::Full]->url = newUrl;

		delete m_extensionRotator;
		m_extensionRotator = nullptr;

		if (before != m_url) {
			if (getExtension(before) != getExtension(m_url)) {
				setFileSize(0, Size::Full);
			}
			emit urlChanged(before, m_url);
		}
	}

	m_loadDetails->deleteLater();
	m_loadDetails = nullptr;
	m_loadedDetails = true;

	refreshTokens();

	emit finishedLoadingTags();
}

/**
 * Try to guess the size of the image in pixels for sorting.
 * @return The guessed number of pixels in the image.
 */
int Image::value() const
{
	QSize size = m_sizes[Image::Size::Full]->size;

	// Get from image size
	if (!size.isEmpty()) {
		return size.width() * size.height();
	}

	// Get from tags
	if (hasTag("incredibly_absurdres")) {
		return 10000 * 10000;
	}
	if (hasTag("absurdres")) {
		return 3200 * 2400;
	}
	if (hasTag("highres")) {
		return 1600 * 1200;
	}
	if (hasTag("lowres")) {
		return 500 * 500;
	}

	return 1200 * 900;
}

Image::SaveResult Image::save(const QString &path, Size size, bool force, bool basic, bool addMd5, bool startCommands, int count, bool postSav)
{
	SaveResult res = SaveResult::Saved;

	QFile f(path);
	if (!f.exists() || force) {
		const QPair<QString, QString> md5action = m_profile->md5Action(md5(), path);
		const QString &whatToDo = md5action.first;
		const QString &md5Duplicate = md5action.second;

		// Only create the destination directory if we're going to put a file there
		if (force || whatToDo != "ignore") {
			const QString p = path.section(QDir::separator(), 0, -2);
			QDir pathToFile(p), dir;
			if (!pathToFile.exists() && !dir.mkpath(p)) {
				log(QStringLiteral("Impossible to create the destination folder: %1.").arg(p), Logger::Error);
				return SaveResult::Error;
			}
		}

		if (whatToDo == "save" || force) {
			const QString savePath = m_sizes[size]->save(path);
			if (!savePath.isEmpty()) {
				log(QStringLiteral("Saving image in `%1` (from `%2`)").arg(path, savePath));
			} else {
				return SaveResult::NotLoaded;
			}
		} else if (whatToDo == "copy") {
			log(QStringLiteral("Copy from `%1` to `%2`").arg(md5Duplicate, path));
			QFile(md5Duplicate).copy(path);

			res = SaveResult::Copied;
		} else if (whatToDo == "move") {
			log(QStringLiteral("Moving from `%1` to `%2`").arg(md5Duplicate, path));
			QFile::rename(md5Duplicate, path);
			m_profile->removeMd5(md5(), md5Duplicate);

			res = SaveResult::Moved;
		} else if (whatToDo == "link" || whatToDo == "hardlink") {
			log(QStringLiteral("Creating %1 for `%2` in `%3`").arg(whatToDo, md5Duplicate, path));
			createLink(md5Duplicate, path, whatToDo);
			res = SaveResult::Linked;
			#ifdef Q_OS_WIN
				if (whatToDo == "link") {
					res = SaveResult::Shortcut;
				}
			#endif
		} else if (!QFile::exists(md5Duplicate)) {
			log(QStringLiteral("MD5 \"%1\" of the image `%2` already found in non-existing file `%3`").arg(md5(), m_url.toString(), md5Duplicate));
			return SaveResult::AlreadyExistsDeletedMd5;
		} else {
			log(QStringLiteral("MD5 \"%1\" of the image `%2` already found in file `%3`").arg(md5(), m_url.toString(), md5Duplicate));
			return SaveResult::AlreadyExistsMd5;
		}

		if (postSav) {
			postSave(path, size, res, addMd5, startCommands, count, basic);
		}
	} else {
		res = SaveResult::AlreadyExistsDisk;
	}

	return res;
}
void Image::postSaving(const QString &path, Size size, bool addMd5, bool startCommands, int count, bool basic)
{
	if (addMd5) {
		m_profile->addMd5(md5(), path);
	}

	// Save info to a text file
	if (!basic) {
		auto logFiles = getExternalLogFiles(m_settings);
		for (auto it = logFiles.constBegin(); it != logFiles.constEnd(); ++it) {
			auto logFile = it.value();
			const Filename textfileFormat = Filename(logFile["content"].toString());
			QStringList cont = textfileFormat.path(*this, m_profile, "", count, Filename::Complex);
			if (!cont.isEmpty()) {
				const int locationType = logFile["locationType"].toInt();
				QString contents = cont.first();

				// File path
				QString fileTagsPath;
				if (locationType == 0) {
					fileTagsPath = this->paths(logFile["filename"].toString(), logFile["path"].toString(), 0).first();
				} else if (locationType == 1) {
					fileTagsPath = logFile["uniquePath"].toString();
				} else if (locationType == 2) {
					fileTagsPath = path + logFile["suffix"].toString();
				}

				// Replace some post-save tokens
				contents.replace("%path:nobackslash%", QDir::toNativeSeparators(path).replace("\\", "/"))
					.replace("%path%", QDir::toNativeSeparators(path));

				// Append to file if necessary
				QFile fileTags(fileTagsPath);
				const bool append = fileTags.exists();
				if (fileTags.open(QFile::WriteOnly | QFile::Append | QFile::Text)) {
					if (append) {
						fileTags.write("\n");
					}
					fileTags.write(contents.toUtf8());
					fileTags.close();
				}
			}
		}
	}

	// Keep original date
	if (m_settings->value("Save/keepDate", true).toBool()) {
		setFileCreationDate(path, createdAt());
	}

	// Commands
	Commands &commands = m_profile->getCommands();
	if (startCommands) {
		commands.before();
	}
	for (const Tag &tag : qAsConst(m_tags)) {
		commands.tag(*this, tag, false);
	}
	commands.image(*this, path);
	for (const Tag &tag : qAsConst(m_tags)) {
		commands.tag(*this, tag, true);
	}
	if (startCommands) {
		commands.after();
	}

	setSavePath(path, size);
}
QMap<QString, Image::SaveResult> Image::save(const QStringList &paths, bool addMd5, bool startCommands, int count, bool force, Size size)
{
	QMap<QString, Image::SaveResult> res;
	for (const QString &path : paths) {
		res.insert(path, save(path, size, force, false, addMd5, startCommands, count));
	}
	return res;
}
QMap<QString, Image::SaveResult> Image::save(const QString &filename, const QString &path, bool addMd5, bool startCommands, int count, Size size)
{
	const QStringList paths = this->paths(filename, path, count);
	return save(paths, addMd5, startCommands, count, false, size);
}

QList<Tag> Image::filteredTags(const QStringList &remove) const
{
	QList<Tag> tags;

	QRegExp reg;
	reg.setCaseSensitivity(Qt::CaseInsensitive);
	reg.setPatternSyntax(QRegExp::Wildcard);
	for (const Tag &tag : m_tags) {
		bool removed = false;
		for (const QString &rem : remove) {
			reg.setPattern(rem);
			if (reg.exactMatch(tag.text())) {
				removed = true;
				break;
			}
		}

		if (!removed) {
			tags.append(tag);
		}
	}

	return tags;
}


Site *Image::parentSite() const { return m_parentSite; }
const QList<Tag> &Image::tags() const { return m_tags; }
const QList<Pool> &Image::pools() const { return m_pools; }
qulonglong Image::id() const { return m_id; }
int Image::fileSize() const { return m_sizes[Image::Size::Full]->fileSize; }
int Image::width() const { return size(Image::Size::Full).width(); }
int Image::height() const { return size(Image::Size::Full).height(); }
const QStringList &Image::search() const { return m_search; }
QDateTime Image::createdAt() const { return token<QDateTime>("date"); }
QString Image::dateRaw() const { return token<QString>("date_raw"); }
const QUrl &Image::fileUrl() const { return m_sizes[Size::Full]->url; }
const QUrl &Image::pageUrl() const { return m_pageUrl; }
QSize Image::size(Size size) const { return m_sizes[size]->size; }
const QString &Image::name() const { return m_name; }
QPixmap Image::previewImage() const { return m_sizes[Image::Size::Thumbnail]->pixmap(); }
const QPixmap &Image::previewImage() { return m_sizes[Image::Size::Thumbnail]->pixmap(); }
Page *Image::page() const { return m_parent; }
const QUrl &Image::parentUrl() const { return m_parentUrl; }
bool Image::isGallery() const { return m_isGallery; }
ExtensionRotator *Image::extensionRotator() const { return m_extensionRotator; }
QString Image::extension() const { return getExtension(m_url).toLower(); }

void Image::setPromoteDetailParsWarn(bool val) { m_detailsParsWarnAsErr = val; }
void Image::setPreviewImage(const QPixmap &preview)
{
	m_sizes[Image::Size::Thumbnail]->setPixmap(preview);
}
void Image::setTemporaryPath(const QString &path, Size size)
{
	if (m_sizes[size]->setTemporaryPath(path)) {
		refreshTokens();
	}
}
void Image::setSavePath(const QString &path, Size size)
{
	if (m_sizes[size]->setSavePath(path)) {
		refreshTokens();
	}
}
QString Image::savePath(Size size) const
{ return m_sizes[size]->savePath(); }

Image::Size Image::preferredDisplaySize() const
{
	const bool getOriginals = m_settings->value("Save/downloadoriginals", true).toBool();
	const bool viewSample = m_settings->value("Zoom/viewSamples", false).toBool();

	return !url(Size::Sample).isEmpty() && (!getOriginals || viewSample)
		? Size::Sample
		: Size::Full;
}

QStringList Image::tagsString() const
{
	QStringList tags;
	tags.reserve(m_tags.count());
	for (const Tag &tag : m_tags) {
		tags.append(tag.text());
	}
	return tags;
}

void Image::setUrl(const QUrl &url)
{
	setFileSize(0, Size::Full); // FIXME
	emit urlChanged(m_url, url);
	m_url = url;
	refreshTokens();
}
void Image::setSize(QSize size, Size s)
{
	m_sizes[s]->size = size;
	refreshTokens();
}
void Image::setFileSize(int fileSize, Size s)
{
	m_sizes[s]->fileSize = fileSize;
	refreshTokens();
}
void Image::setTags(const QList<Tag> &tags)
{
	m_tags = tags;
	refreshTokens();
}
void Image::setParentGallery(const QSharedPointer<Image> &parentGallery)
{
	m_parentGallery = parentGallery;
	refreshTokens();
}

QColor Image::color() const
{
	// Blacklisted
	QStringList detected = m_profile->getBlacklist().match(tokens(m_profile));
	if (!detected.isEmpty()) {
		return { 0, 0, 0 };
	}

	// Favorited (except for exact favorite search)
	auto favorites = m_profile->getFavorites();
	for (const Tag &tag : m_tags) {
		if (!m_parent->search().contains(tag.text())) {
			for (const Favorite &fav : favorites) {
				if (fav.getName() == tag.text()) {
					return { 255, 192, 203 };
				}
			}
		}
	}

	// Image with a parent
	if (token<int>("parentid") != 0) {
		return { 204, 204, 0 };
	}

	// Image with children
	if (token<bool>("has_children")) {
		return { 0, 255, 0 };
	}

	// Pending image
	if (token<QString>("status") == "pending") {
		return { 0, 0, 255 };
	}

	return {};
}

QString Image::tooltip() const
{
	if (m_isGallery) {
		return QStringLiteral("%1%2")
			.arg(m_id == 0 ? " " : tr("<b>ID:</b> %1<br/>").arg(m_id))
			.arg(m_name.isEmpty() ? " " : tr("<b>Name:</b> %1<br/>").arg(m_name));
	}

	double size = m_sizes[Image::Size::Full]->fileSize;
	const QString unit = getUnit(&size);

	const QString &rating = token<QString>("rating");
	const QDateTime &createdAt = token<QDateTime>("date");
	const QString &author = token<QString>("author");
	const QString &score = token<QString>("score");

	return QStringLiteral("%1%2%3%4%5%6%7%8")
		.arg(m_tags.isEmpty() ? " " : tr("<b>Tags:</b> %1<br/><br/>").arg(TagStylist(m_profile).stylished(m_tags, false, false, m_settings->value("Zoom/tagOrder", "type").toString()).join(' ')))
		.arg(m_id == 0 ? " " : tr("<b>ID:</b> %1<br/>").arg(m_id))
		.arg(rating.isEmpty() ? " " : tr("<b>Rating:</b> %1<br/>").arg(rating))
		.arg(!score.isEmpty() ? tr("<b>Score:</b> %1<br/>").arg(score) : " ")
		.arg(author.isEmpty() ? " " : tr("<b>User:</b> %1<br/><br/>").arg(author))
		.arg(width() == 0 || height() == 0 ? " " : tr("<b>Size:</b> %1 x %2<br/>").arg(QString::number(width()), QString::number(height())))
		.arg(m_sizes[Image::Size::Full]->fileSize == 0 ? " " : tr("<b>Filesize:</b> %1 %2<br/>").arg(QString::number(size), unit))
		.arg(!createdAt.isValid() ? " " : tr("<b>Date:</b> %1").arg(createdAt.toString(Qt::DefaultLocaleShortDate)));
}

QString Image::counter() const
{
	return m_galleryCount > 0 ? QString::number(m_galleryCount) : QString();
}

QList<QStrP> Image::detailsData() const
{
	const QString unknown = tr("<i>Unknown</i>");
	const QString yes = tr("yes");
	const QString no = tr("no");

	QString sources;
	for (const QString &source : m_sources) {
		sources += (!sources.isEmpty() ? "<br/>" : "") + QString("<a href=\"%1\">%1</a>").arg(source);
	}

	const QString &rating = token<QString>("rating");
	const QDateTime &createdAt = token<QDateTime>("date");
	const QString &author = token<QString>("author");
	int parentId = token<int>("parentid");

	return {
		QStrP(tr("Tags"), TagStylist(m_profile).stylished(m_tags, false, false, m_settings->value("Zoom/tagOrder", "type").toString()).join(' ')),
		QStrP(),
		QStrP(tr("ID"), m_id != 0 ? QString::number(m_id) : unknown),
		QStrP(tr("MD5"), !m_md5.isEmpty() ? m_md5 : unknown),
		QStrP(tr("Rating"), !rating.isEmpty() ? rating : unknown),
		QStrP(tr("Score"), token<QString>("score")),
		QStrP(tr("Author"), !author.isEmpty() ? author : unknown),
		QStrP(),
		QStrP(tr("Date"), createdAt.isValid() ? createdAt.toString(Qt::DefaultLocaleShortDate) : unknown),
		QStrP(tr("Size"), !size().isEmpty() ? QString::number(width()) + "x" + QString::number(height()) : unknown),
		QStrP(tr("Filesize"), m_sizes[Image::Size::Full]->fileSize != 0 ? formatFilesize(m_sizes[Image::Size::Full]->fileSize) : unknown),
		QStrP(),
		QStrP(tr("Page"), !m_pageUrl.isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(m_pageUrl.toString()) : unknown),
		QStrP(tr("URL"), !m_sizes[Size::Full]->url.isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(m_sizes[Size::Full]->url.toString()) : unknown),
		QStrP(tr("Source(s)", "", m_sources.count()), !sources.isEmpty() ? sources : unknown),
		QStrP(tr("Sample"), !url(Size::Sample).isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(url(Size::Sample).toString()) : unknown),
		QStrP(tr("Thumbnail"), !url(Size::Thumbnail).isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(url(Size::Thumbnail).toString()) : unknown),
		QStrP(),
		QStrP(tr("Parent"), parentId != 0 ? tr("yes (#%1)").arg(parentId) : no),
		QStrP(tr("Comments"), token<bool>("has_comments") ? yes : no),
		QStrP(tr("Children"), token<bool>("has_children") ? yes : no),
		QStrP(tr("Notes"), token<bool>("has_note") ? yes : no),
	};
}

QString Image::md5() const
{
	if (m_md5.isEmpty()) {
		return md5forced();
	}
	return m_md5;
}
QString Image::md5forced() const
{
	return m_sizes[Image::Size::Full]->md5();
}

bool Image::hasTag(QString tag) const
{
	tag = tag.trimmed();
	for (const Tag &t : m_tags) {
		if (QString::compare(t.text(), tag, Qt::CaseInsensitive) == 0) {
			return true;
		}
	}
	return false;
}
bool Image::hasUnknownTag() const
{
	if (m_tags.isEmpty()) {
		return true;
	}
	for (const Tag &tag : qAsConst(m_tags)) {
		if (tag.type().isUnknown()) {
			return true;
		}
	}
	return false;
}

void Image::setFileExtension(const QString &ext)
{
	m_url = setExtension(m_url, ext);
	m_sizes[Size::Full]->url = setExtension(m_sizes[Size::Full]->url, ext);
	refreshTokens();
}

bool Image::isVideo() const
{
	const QString ext = getExtension(m_url).toLower();
	return ext == "mp4" || ext == "webm";
}
QString Image::isAnimated() const
{
	QString ext = getExtension(m_url).toLower();

	if (ext == "gif" || ext == "apng") {
		return ext;
	}

	if (ext == "png" && (hasTag(QStringLiteral("animated")) || hasTag(QStringLiteral("animated_png")))) {
		return QStringLiteral("apng");
	}

	return QString();
}


QUrl Image::url(Size size) const
{
	if (size == Size::Full) {
		return m_url;
	}
	return m_sizes[size]->url;
}

void Image::preload(const Filename &filename)
{
	if (filename.needExactTags(m_parentSite, m_settings) == 0) {
		return;
	}

	QEventLoop loop;
	QObject::connect(this, &Image::finishedLoadingTags, &loop, &QEventLoop::quit);
	loadDetails();
	loop.exec();
}

QStringList Image::paths(const QString &filename, const QString &folder, int count) const
{
	return paths(Filename(filename), folder, count);
}
QStringList Image::paths(const Filename &filename, const QString &folder, int count) const
{
	return filename.path(*this, m_profile, folder, count, Filename::Complex | Filename::Path);
}

QMap<QString, Token> Image::generateTokens(Profile *profile) const
{
	QSettings *settings = profile->getSettings();
	QStringList ignore = profile->getIgnored();
	const QStringList remove = settings->value("ignoredtags").toString().split(' ', QString::SkipEmptyParts);

	QMap<QString, Token> tokens;

	// Pool
	static const QRegularExpression poolRegexp("pool:(\\d+)");
	QRegularExpressionMatch poolMatch = poolRegexp.match(m_search.join(' '));
	tokens.insert("pool", Token(poolMatch.hasMatch() ? poolMatch.captured(1) : "", ""));

	// Metadata
	tokens.insert("filename", Token(QUrl::fromPercentEncoding(m_url.fileName().section('.', 0, -2).toUtf8()), ""));
	tokens.insert("website", Token(m_parentSite->url()));
	tokens.insert("websitename", Token(m_parentSite->name()));
	tokens.insert("md5", Token(md5()));
	tokens.insert("md5_forced", Token([this]() { return this->md5forced(); }));
	tokens.insert("id", Token(m_id));
	tokens.insert("height", Token(height()));
	tokens.insert("width", Token(width()));
	tokens.insert("mpixels", Token(width() * height()));
	tokens.insert("url_file", Token(m_url));
	tokens.insert("url_original", Token(m_sizes[Size::Full]->url.toString()));
	tokens.insert("url_sample", Token(url(Size::Sample).toString()));
	tokens.insert("url_thumbnail", Token(url(Size::Thumbnail).toString()));
	tokens.insert("url_page", Token(m_pageUrl.toString()));
	tokens.insert("source", Token(!m_sources.isEmpty() ? m_sources.first() : ""));
	tokens.insert("sources", Token(m_sources));
	tokens.insert("filesize", Token(m_sizes[Image::Size::Full]->fileSize));
	tokens.insert("name", Token(m_name));
	tokens.insert("position", Token(m_position > 0 ? QString::number(m_position) : ""));

	// Search
	for (int i = 0; i < m_search.size(); ++i) {
		tokens.insert("search_" + QString::number(i + 1), Token(m_search[i]));
	}
	for (int i = m_search.size(); i < 10; ++i) {
		tokens.insert("search_" + QString::number(i + 1), Token(""));
	}
	tokens.insert("search", Token(m_search.join(' ')));

	// Tags
	const auto tags = filteredTags(remove);
	QMap<QString, QStringList> details;
	for (const Tag &tag : tags) {
		const QString &t = tag.text();

		details[ignore.contains(t, Qt::CaseInsensitive) ? "general" : tag.type().name()].append(t);
		details["alls"].append(t);
		details["alls_namespaces"].append(tag.type().name());

		QString underscored = QString(t);
		underscored.replace(' ', '_');
		details["allos"].append(underscored);
	}

	// Shorten copyrights
	if (settings->value("Save/copyright_useshorter", true).toBool()) {
		QStringList copyrights;
		for (const QString &cop : details["copyright"]) {
			bool found = false;
			for (QString &copyright : copyrights) {
				if (copyright.left(cop.size()) == cop.left(copyright.size())) {
					if (cop.size() < copyright.size()) {
						copyright = cop;
					}
					found = true;
				}
			}
			if (!found) {
				copyrights.append(cop);
			}
		}
		details["copyright"] = copyrights;
	}

	// Tags
	tokens.insert("general", Token(details["general"]));
	tokens.insert("artist", Token(details["artist"], "keepAll", "anonymous", "multiple artists"));
	tokens.insert("copyright", Token(details["copyright"], "keepAll", "misc", "crossover"));
	tokens.insert("character", Token(details["character"], "keepAll", "unknown", "group"));
	tokens.insert("model", Token(details["model"] + details["idol"], "keepAll", "unknown", "multiple"));
	tokens.insert("photo_set", Token(details["photo_set"], "keepAll", "unknown", "multiple"));
	tokens.insert("species", Token(details["species"], "keepAll", "unknown", "multiple"));
	tokens.insert("meta", Token(details["meta"], "keepAll", "none", "multiple"));
	tokens.insert("allos", Token(details["allos"]));
	tokens.insert("allo", Token(details["allos"].join(' ')));
	tokens.insert("tags", Token(QVariant::fromValue(tags)));
	tokens.insert("all", Token(details["alls"]));
	tokens.insert("all_namespaces", Token(details["alls_namespaces"]));

	// Extension
	QString ext = extension();
	if (settings->value("Save/noJpeg", true).toBool() && ext == "jpeg") {
		ext = "jpg";
	}
	tokens.insert("ext", Token(ext, "jpg"));
	tokens.insert("filetype", Token(ext, "jpg"));

	// Variables
	if (!m_parentGallery.isNull()) {
		tokens.insert("gallery", Token([this, profile]() { return QVariant::fromValue(m_parentGallery->tokens(profile)); }));
	}

	// Extra tokens
	static const QVariantMap defaultValues
	{
		{ "rating", "unknown" },
	};
	for (auto it = m_data.constBegin(); it != m_data.constEnd(); ++it) {
		tokens.insert(it.key(), Token(it.value(), defaultValues.value(it.key())));
	}

	return tokens;
}

Image::SaveResult Image::preSave(const QString &path, Size size)
{
	return save(path, size, false, false, false, false, 1, false);
}

void Image::postSave(const QString &path, Size size, SaveResult res, bool addMd5, bool startCommands, int count, bool basic)
{
	static const QList<SaveResult> md5Results { SaveResult::Moved, SaveResult::Copied, SaveResult::Shortcut, SaveResult::Linked, SaveResult::Saved };
	postSaving(path, size, addMd5 && md5Results.contains(res), startCommands, count, basic);
}
