#include <QCryptographicHash>
#include <QEventLoop>
#include <QJsonArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QSettings>
#include <QTimer>
#include "commands/commands.h"
#include "downloader/extension-rotator.h"
#include "favorite.h"
#include "functions.h"
#include "loader/token.h"
#include "models/api/api.h"
#include "models/filename.h"
#include "models/image.h"
#include "models/page.h"
#include "models/pool.h"
#include "models/profile.h"
#include "models/site.h"
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
	m_isGallery = other.m_isGallery;

	m_id = other.m_id;
	m_score = other.m_score;
	m_parentId = other.m_parentId;
	m_authorId = other.m_authorId;

	m_hasChildren = other.m_hasChildren;
	m_hasNote = other.m_hasNote;
	m_hasComments = other.m_hasComments;
	m_hasScore = other.m_hasScore;

	m_url = other.m_url;
	m_md5 = other.m_md5;
	m_author = other.m_author;
	m_name = other.m_name;
	m_status = other.m_status;
	m_rating = other.m_rating;
	m_sources = other.m_sources;

	m_pageUrl = other.m_pageUrl;
	m_fileUrl = other.m_fileUrl;
	m_sampleUrl = other.m_sampleUrl;
	m_previewUrl = other.m_previewUrl;

	m_sizes = other.m_sizes;
	m_createdAt = other.m_createdAt;

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


Image::Image(Site *site, QMap<QString, QString> details, Profile *profile, Page *parent)
	: m_profile(profile), m_id(0), m_parentSite(site), m_extensionRotator(nullptr)
{
	m_settings = m_profile->getSettings();

	// Parents
	if (m_parentSite == nullptr) {
		log(QStringLiteral("Image has nullptr parent, aborting creation."));
		return;
	}

	// Other details
	m_isGallery = details.contains("type") && details["type"] == "gallery";
	m_md5 = details.contains("md5") ? details["md5"] : "";
	m_author = details.contains("author") ? details["author"] : "";
	m_name = details.contains("name") ? details["name"] : "";
	m_status = details.contains("status") ? details["status"] : "";
	m_search = parent != nullptr ? parent->search() : (details.contains("search") ? details["search"].split(' ') : QStringList());
	m_id = details.contains("id") ? details["id"].toULongLong() : 0;
	m_score = details.contains("score") ? details["score"].toInt() : 0;
	m_hasScore = details.contains("score");
	m_parentId = details.contains("parent_id") ? details["parent_id"].toInt() : 0;
	m_authorId = details.contains("creator_id") ? details["creator_id"].toInt() : 0;
	m_hasChildren = details.contains("has_children") && details["has_children"] == "true";
	m_hasNote = details.contains("has_note") && details["has_note"] == "true";
	m_hasComments = details.contains("has_comments") && details["has_comments"] == "true";
	m_fileUrl = details.contains("file_url") ? m_parentSite->fixUrl(details["file_url"]) : QUrl();
	m_sampleUrl = details.contains("sample_url") ? m_parentSite->fixUrl(details["sample_url"]) : QUrl();
	m_previewUrl = details.contains("preview_url") ? m_parentSite->fixUrl(details["preview_url"]) : QUrl();
	m_sources = details.contains("sources") ? details["sources"].split('\n') : (details.contains("source") ? QStringList() << details["source"] : QStringList());
	m_galleryCount = details.contains("gallery_count") ? details["gallery_count"].toInt() : -1;
	m_position = details.contains("position") ? details["position"].toInt() : 0;

	// Sizes
	static QMap<Image::Size, QString> prefixes =
	{
		{ Image::Size::Full, "" },
		{ Image::Size::Sample, "sample_" },
		{ Image::Size::Thumbnail, "preview_" },
	};
	for (auto it = prefixes.constBegin(); it != prefixes.constEnd(); ++it) {
		const QString &prefix = it.value();

		auto is = QSharedPointer<ImageSize>::create();

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
	} else {
		Api *api = m_parentSite->detailsApi();
		if (api != nullptr) {
			m_pageUrl = api->detailsUrl(m_id, m_md5, m_parentSite).url;
		}
	}
	m_pageUrl = site->fixUrl(m_pageUrl).toString();

	// Rating
	setRating(details.contains("rating") ? details["rating"] : "");

	// Tags
	QStringList types = QStringList() << "general" << "artist" << "character" << "copyright" << "model" << "species" << "meta";
	for (const QString &typ : types) {
		const QString key = "tags_" + typ;
		if (!details.contains(key)) {
			continue;
		}

		const TagType ttype(typ);
		QStringList t = details[key].split(' ', QString::SkipEmptyParts);
		for (QString tg : t) {
			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, ttype));
		}
	}
	if (m_tags.isEmpty() && details.contains("tags")) {
		QString tgs = QString(details["tags"]).replace(QRegularExpression("[\r\n\t]+"), " ");

		// Automatically find tag separator and split the list
		const int commas = tgs.count(", ");
		const int spaces = tgs.count(" ");
		const QStringList &t = commas >= 10 || (commas > 0 && (spaces - commas) / commas < 2)
			? tgs.split(", ", QString::SkipEmptyParts)
			: tgs.split(" ", QString::SkipEmptyParts);

		for (QString tg : t) {
			tg.replace("&amp;", "&");

			const int colon = tg.indexOf(':');
			if (colon != -1) {
				const QString tp = tg.left(colon).toLower();
				if (tp == "user") {
					m_author = tg.mid(colon + 1);
				} else if (tp == "score") {
					m_score = tg.midRef(colon + 1).toInt();
				} else if (tp == "size") {
					QStringList size = tg.mid(colon + 1).split('x');
					if (size.size() == 2) {
						setSize(QSize(size[0].toInt(), size[1].toInt()), Size::Full);
					}
				} else if (tp == "rating") {
					setRating(tg.mid(colon + 1));
				} else {
					m_tags.append(Tag(tg));
				}
			} else {
				m_tags.append(Tag(tg));
			}
		}
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
	m_url = m_fileUrl;
	const QString ext = getExtension(m_url);
	if (details.contains("ext") && !details["ext"].isEmpty()) {
		const QString realExt = details["ext"];
		if (ext != realExt) {
			setFileExtension(realExt);
		}
	} else if (ext == QLatin1String("jpg") && !m_previewUrl.isEmpty()) {
		bool fixed = false;
		const QString previewExt = getExtension(QUrl(details["preview_url"]));
		if (!m_sampleUrl.isEmpty()) {
			// Guess extension from sample url
			const QString sampleExt = getExtension(QUrl(details["sample_url"]));
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
	}

	// Remove ? in urls
	m_url = removeCacheBuster(m_url);
	m_fileUrl = removeCacheBuster(m_fileUrl);
	m_sampleUrl = removeCacheBuster(m_sampleUrl);
	m_previewUrl = removeCacheBuster(m_previewUrl);

	// We use the sample URL as the URL for zip files (ugoira) or if the setting is set
	const bool downloadOriginals = m_settings->value("Save/downloadoriginals", true).toBool();
	if (!m_sampleUrl.isEmpty() && (getExtension(m_url) == "zip" || !downloadOriginals)) {
		m_url = m_sampleUrl.toString();
	}

	// Creation date
	m_createdAt = QDateTime();
	if (details.contains("created_at")) {
		m_createdAt = qDateTimeFromString(details["created_at"]);
	} else if (details.contains("date")) {
		m_createdAt = QDateTime::fromString(details["date"], Qt::ISODate);
	}

	// Setup extension rotator
	const bool animated = hasTag("gif") || hasTag("animated_gif") || hasTag("mp4") || hasTag("animated_png") || hasTag("webm") || hasTag("animated");
	const QStringList extensions = animated
		? QStringList() << "webm" << "mp4" << "gif" << "jpg" << "png" << "jpeg" << "swf"
		: QStringList() << "jpg" << "png" << "gif" << "jpeg" << "webm" << "swf" << "mp4";
	m_extensionRotator = new ExtensionRotator(getExtension(m_url), extensions, this);

	// Tech details
	m_parent = parent;
	m_loadDetails = nullptr;
	m_loadingDetails = false;
	m_loadedDetails = false;
	m_pools = QList<Pool>();
}


void Image::write(QJsonObject &json) const
{
	json["website"] = m_parentSite->url();

	if (!m_parentGallery.isNull()) {
		QJsonObject jsonGallery;
		m_parentGallery->write(jsonGallery);
		json["gallery"] = jsonGallery;
	}

	QStringList tags;
	tags.reserve(m_tags.count());
	for (const Tag &tag : m_tags) {
		tags.append(tag.text());
	}

	// FIXME: real serialization
	json["name"] = m_name;
	json["id"] = static_cast<int>(m_id);
	json["md5"] = m_md5;
	json["rating"] = m_rating;
	json["tags"] = QJsonArray::fromStringList(tags);
	json["file_url"] = m_fileUrl.toString();
	json["date"] = m_createdAt.toString(Qt::ISODate);
	json["search"] = QJsonArray::fromStringList(m_search);
}

bool Image::read(const QJsonObject &json, const QMap<QString, Site*> &sites)
{
	const QString site = json["website"].toString();
	if (!sites.contains(site)) {
		return false;
	}

	if (json.contains("gallery")) {
		auto gallery = new Image();
		if (gallery->read(json["gallery"].toObject(), sites)) {
			m_parentGallery = QSharedPointer<Image>(gallery);
		} else {
			gallery->deleteLater();
			return false;
		}
	}

	m_parentSite = sites[site];
	using ::savePath;
	m_profile = new Profile(savePath()); // FIXME

	m_name = json["name"].toString();
	m_id = json["id"].toInt();
	m_md5 = json["md5"].toString();
	m_rating = json["rating"].toString();
	m_fileUrl = json["file_url"].toString();
	m_createdAt = QDateTime::fromString(json["date"].toString(), Qt::ISODate);

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

	m_sizes = {
		{ Image::Size::Full, QSharedPointer<ImageSize>::create() },
		{ Image::Size::Sample, QSharedPointer<ImageSize>::create() },
		{ Image::Size::Thumbnail, QSharedPointer<ImageSize>::create() },
	};

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

	// Load the request with a possible delay
	const int ms = m_parentSite->msToRequest(rateLimit ? Site::QueryType::Retry : Site::QueryType::List);
	if (ms > 0) {
		QTimer::singleShot(ms, this, SLOT(loadDetailsNow()));
	} else {
		loadDetailsNow();
	}
}
void Image::loadDetailsNow()
{
	if (m_loadDetails != nullptr) {
		if (m_loadDetails->isRunning()) {
			m_loadDetails->abort();
		}

		m_loadDetails->deleteLater();
	}

	m_loadDetails = m_parentSite->get(m_pageUrl);
	m_loadDetails->setParent(this);
	m_loadingDetails = true;

	connect(m_loadDetails, &QNetworkReply::finished, this, &Image::parseDetails);
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

	// Aborted or connection error
	if (m_loadDetails->error()) {
		if (m_loadDetails->error() != QNetworkReply::OperationCanceledError) {
			log(QStringLiteral("Loading error for '%1': %2").arg(m_pageUrl.toString(), m_loadDetails->errorString()), Logger::Error);
		}
		m_loadDetails->deleteLater();
		m_loadDetails = nullptr;
		return;
	}

	// Check redirection
	QUrl redir = m_loadDetails->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty()) {
		m_pageUrl = redir;
		loadDetails();
		return;
	}

	const int statusCode = m_loadDetails->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 429) {
		log(QStringLiteral("Details limit reached (429). New try."));
		loadDetails(true);
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
		m_createdAt = ret.createdAt;
	}

	// Image url
	if (!ret.imageUrl.isEmpty()) {
		const QUrl before = m_url;
		const QUrl newUrl = m_parentSite->fixUrl(ret.imageUrl, before);

		m_url = newUrl;
		m_fileUrl = newUrl;

		delete m_extensionRotator;
		m_extensionRotator = nullptr;

		if (before != m_url) {
			if (getExtension(before) != getExtension(m_url)) {
				setFileSize(0, Size::Full);
			}
			emit urlChanged(before, m_url);
		}
	}

	// Get rating from tags
	if (m_rating.isEmpty()) {
		int ratingTagIndex = -1;
		for (int it = 0; it < m_tags.count(); ++it) {
			if (m_tags[it].type().name() == "rating") {
				m_rating = m_tags[it].text();
				ratingTagIndex = it;
				break;
			}
		}
		if (ratingTagIndex != -1) {
			m_tags.removeAt(ratingTagIndex);
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

Image::SaveResult Image::save(const QString &path, Size size, bool force, bool basic, bool addMd5, bool startCommands, int count, bool postSave)
{
	SaveResult res = SaveResult::Saved;

	QFile f(path);
	if (!f.exists() || force) {
		const QPair<QString, QString> md5action = m_profile->md5Action(md5());
		const QString &whatToDo = md5action.first;
		const QString &md5Duplicate = md5action.second;

		// Only create the destination directory if we're going to put a file there
		if (md5Duplicate.isEmpty() || force || whatToDo != "ignore") {
			const QString p = path.section(QDir::separator(), 0, -2);
			QDir pathToFile(p), dir;
			if (!pathToFile.exists() && !dir.mkpath(p)) {
				log(QStringLiteral("Impossible to create the destination folder: %1.").arg(p), Logger::Error);
				return SaveResult::Error;
			}
		}

		if (md5Duplicate.isEmpty() || whatToDo == "save" || force) {
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
			m_profile->setMd5(md5(), path);

			res = SaveResult::Moved;
		} else if (whatToDo == "link") {
			log(QStringLiteral("Creating link for `%1` in `%2`").arg(md5Duplicate, path));

			#ifdef Q_OS_WIN
				QFile::link(md5Duplicate, path + ".lnk");
			#else
				QFile::link(md5Duplicate, path);
			#endif

			res = SaveResult::Linked;
		} else {
			log(QStringLiteral("MD5 \"%1\" of the image `%2` already found in file `%3`").arg(md5(), m_url.toString(), md5Duplicate));
			return SaveResult::AlreadyExistsMd5;
		}

		if (postSave) {
			postSaving(path, size, addMd5 && res == SaveResult::Saved, startCommands, count, basic);
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
	if (startCommands)
	{ commands.before(); }
		for (const Tag &tag : qAsConst(m_tags))
		{ commands.tag(*this, tag, false); }
		commands.image(*this, path);
		for (const Tag &tag : qAsConst(m_tags))
		{ commands.tag(*this, tag, true); }
	if (startCommands)
	{ commands.after(); }

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
const QString &Image::rating() const { return m_rating; }
const QStringList &Image::search() const { return m_search; }
const QDateTime &Image::createdAt() const { return m_createdAt; }
const QUrl &Image::fileUrl() const { return m_fileUrl; }
const QUrl &Image::pageUrl() const { return m_pageUrl; }
QSize Image::size(Size size) const { return m_sizes[size]->size; }
const QString &Image::name() const { return m_name; }
QPixmap Image::previewImage() const { return m_sizes[Image::Size::Thumbnail]->pixmap(); }
const QPixmap &Image::previewImage() { return m_sizes[Image::Size::Thumbnail]->pixmap(); }
Page *Image::page() const { return m_parent; }
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

	return !m_sampleUrl.isEmpty() && (!getOriginals || viewSample)
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
	if (m_parentId != 0) {
		return { 204, 204, 0 };
	}

	// Image with children
	if (m_hasChildren) {
		return { 0, 255, 0 };
	}

	// Pending image
	if (m_status == "pending") {
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

	return QStringLiteral("%1%2%3%4%5%6%7%8")
		.arg(m_tags.isEmpty() ? " " : tr("<b>Tags:</b> %1<br/><br/>").arg(TagStylist(m_profile).stylished(m_tags, false, false, m_settings->value("Zoom/tagOrder", "type").toString()).join(' ')))
		.arg(m_id == 0 ? " " : tr("<b>ID:</b> %1<br/>").arg(m_id))
		.arg(m_rating.isEmpty() ? " " : tr("<b>Rating:</b> %1<br/>").arg(m_rating))
		.arg(m_hasScore ? tr("<b>Score:</b> %1<br/>").arg(m_score) : " ")
		.arg(m_author.isEmpty() ? " " : tr("<b>User:</b> %1<br/><br/>").arg(m_author))
		.arg(width() == 0 || height() == 0 ? " " : tr("<b>Size:</b> %1 x %2<br/>").arg(QString::number(width()), QString::number(height())))
		.arg(m_sizes[Image::Size::Full]->fileSize == 0 ? " " : tr("<b>Filesize:</b> %1 %2<br/>").arg(QString::number(size), unit))
		.arg(!m_createdAt.isValid() ? " " : tr("<b>Date:</b> %1").arg(m_createdAt.toString(tr("'the 'MM/dd/yyyy' at 'hh:mm"))));
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

	return
	{
		QStrP(tr("Tags"), TagStylist(m_profile).stylished(m_tags, false, false, m_settings->value("Zoom/tagOrder", "type").toString()).join(' ')),
		QStrP(),
		QStrP(tr("ID"), m_id != 0 ? QString::number(m_id) : unknown),
		QStrP(tr("MD5"), !m_md5.isEmpty() ? m_md5 : unknown),
		QStrP(tr("Rating"), !m_rating.isEmpty() ? m_rating : unknown),
		QStrP(tr("Score"), QString::number(m_score)),
		QStrP(tr("Author"), !m_author.isEmpty() ? m_author : unknown),
		QStrP(),
		QStrP(tr("Date"), m_createdAt.isValid() ? m_createdAt.toString(tr("'the' MM/dd/yyyy 'at' hh:mm")) : unknown),
		QStrP(tr("Size"), !size().isEmpty() ? QString::number(width()) + "x" + QString::number(height()) : unknown),
		QStrP(tr("Filesize"), m_sizes[Image::Size::Full]->fileSize != 0 ? formatFilesize(m_sizes[Image::Size::Full]->fileSize) : unknown),
		QStrP(),
		QStrP(tr("Page"), !m_pageUrl.isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(m_pageUrl.toString()) : unknown),
		QStrP(tr("URL"), !m_fileUrl.isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(m_fileUrl.toString()) : unknown),
		QStrP(tr("Source(s)", "", m_sources.count()), !sources.isEmpty() ? sources : unknown),
		QStrP(tr("Sample"), !m_sampleUrl.isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(m_sampleUrl.toString()) : unknown),
		QStrP(tr("Thumbnail"), !m_previewUrl.isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(m_previewUrl.toString()) : unknown),
		QStrP(),
		QStrP(tr("Parent"), m_parentId != 0 ? tr("yes (#%1)").arg(m_parentId) : no),
		QStrP(tr("Comments"), m_hasComments ? yes : no),
		QStrP(tr("Children"), m_hasChildren ? yes : no),
		QStrP(tr("Notes"), m_hasNote ? yes : no),
	};
}

QString Image::md5() const
{
	const QString savePath = m_sizes[Image::Size::Full]->savePath();

	// If we know the path to the image or its content but not its md5, we calculate it first
	if (m_md5.isEmpty() && !savePath.isEmpty()) {
		QCryptographicHash hash(QCryptographicHash::Md5);

		// Calculate from image path
		QFile f(savePath);
		f.open(QFile::ReadOnly);
		hash.addData(&f);
		f.close();

		m_md5 = hash.result().toHex();
	}

	return m_md5;
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

void Image::setRating(const QString &rating)
{
	static QMap<QString, QString> assoc =
	{
		{ "s", "safe" },
		{ "q", "questionable" },
		{ "e", "explicit" }
	};

	m_rating = assoc.contains(rating)
		? assoc[rating]
		: rating.toLower();

	refreshTokens();
}

void Image::setFileExtension(const QString &ext)
{
	m_url = setExtension(m_url, ext);
	m_fileUrl = setExtension(m_fileUrl, ext);
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
	switch (size)
	{
		case Size::Thumbnail: return m_previewUrl;
		case Size::Sample: return m_sampleUrl;
		default: return m_url;
	}
}

void Image::preload(const Filename &filename)
{
	if (filename.needExactTags(m_parentSite) == 0) {
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
	QRegularExpression poolRegexp("pool:(\\d+)");
	QRegularExpressionMatch poolMatch = poolRegexp.match(m_search.join(' '));
	tokens.insert("pool", Token(poolMatch.hasMatch() ? poolMatch.captured(1) : "", ""));

	// Metadata
	tokens.insert("filename", Token(QUrl::fromPercentEncoding(m_url.fileName().section('.', 0, -2).toUtf8()), ""));
	tokens.insert("website", Token(m_parentSite->url()));
	tokens.insert("websitename", Token(m_parentSite->name()));
	tokens.insert("md5", Token(md5()));
	tokens.insert("date", Token(m_createdAt));
	tokens.insert("id", Token(m_id));
	tokens.insert("rating", Token(m_rating, "unknown"));
	tokens.insert("score", Token(m_score));
	tokens.insert("height", Token(height()));
	tokens.insert("width", Token(width()));
	tokens.insert("mpixels", Token(width() * height()));
	tokens.insert("url_file", Token(m_url));
	tokens.insert("url_original", Token(m_fileUrl.toString()));
	tokens.insert("url_sample", Token(m_sampleUrl.toString()));
	tokens.insert("url_thumbnail", Token(m_previewUrl.toString()));
	tokens.insert("url_page", Token(m_pageUrl.toString()));
	tokens.insert("source", Token(!m_sources.isEmpty() ? m_sources.first() : ""));
	tokens.insert("sources", Token(m_sources));
	tokens.insert("filesize", Token(m_sizes[Image::Size::Full]->fileSize));
	tokens.insert("author", Token(m_author));
	tokens.insert("authorid", Token(m_authorId));
	tokens.insert("parentid", Token(m_parentId));
	tokens.insert("name", Token(m_name));
	tokens.insert("position", Token(m_position > 0 ? QString::number(m_position) : ""));

	// Flags
	tokens.insert("has_children", Token(m_hasChildren));
	tokens.insert("has_note", Token(m_hasNote));
	tokens.insert("has_comments", Token(m_hasComments));

	// Search
	for (int i = 0; i < m_search.size(); ++i) {
		tokens.insert("search_" + QString::number(i + 1), Token(m_search[i]));
	}
	for (int i = m_search.size(); i < 10; ++i) {
		tokens.insert("search_" + QString::number(i + 1), Token(""));
	}
	tokens.insert("search", Token(m_search.join(' ')));

	// Tags
	QMap<QString, QStringList> details;
	for (const Tag &tag : filteredTags(remove)) {
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
	tokens.insert("tags", Token(details["alls"]));
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

	return tokens;
}

Image::SaveResult Image::preSave(const QString &path, Size size)
{
	return save(path, size, false, false, false, false, 1, false);
}

void Image::postSave(const QString &path, Size size, SaveResult res, bool addMd5, bool startCommands, int count)
{
	postSaving(path, size, addMd5 && res == SaveResult::Saved, startCommands, count);
}
