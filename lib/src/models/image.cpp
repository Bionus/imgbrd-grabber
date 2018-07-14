#include <QCryptographicHash>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include "commands/commands.h"
#include "downloader/extension-rotator.h"
#include "favorite.h"
#include "functions.h"
#include "loader/token.h"
#include "models/api/api.h"
#include "models/filename.h"
#include "models/filtering/post-filter.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "tags/tag-database.h"
#include "tags/tag-stylist.h"
#include "tags/tag-type.h"

#define MAX_LOAD_FILESIZE (1024*1024*50)


QUrl removeCacheUrl(QUrl url)
{
	const QString query = url.query();
	if (query.isEmpty())
		return url;

	// Only remove ?integer
	bool ok;
	query.toInt(&ok);
	if (ok)
		url.setQuery(QString());

	return url;
}

Image::Image()
	: m_profile(nullptr), m_extensionRotator(nullptr)
{ }

// TODO(Bionus): clean up this mess
Image::Image(const Image &other)
	: QObject(other.parent())
{
	m_parent = other.m_parent;
	m_isGallery = other.m_isGallery;

	m_id = other.m_id;
	m_score = other.m_score;
	m_parentId = other.m_parentId;
	m_fileSize = other.m_fileSize;
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
	m_source = other.m_source;
	m_savePath = other.m_savePath;

	m_pageUrl = other.m_pageUrl;
	m_fileUrl = other.m_fileUrl;
	m_sampleUrl = other.m_sampleUrl;
	m_previewUrl = other.m_previewUrl;

	m_size = other.m_size;
	m_imagePreview = other.m_imagePreview;
	m_createdAt = other.m_createdAt;
	m_data = other.m_data;

	m_loadDetails = other.m_loadDetails;

	m_tags = other.m_tags;
	m_pools = other.m_pools;
	m_timer = other.m_timer;
	m_profile = other.m_profile;
	m_settings = other.m_settings;
	m_search = other.m_search;
	m_parentSite = other.m_parentSite;

	m_extensionRotator = other.m_extensionRotator;
	m_loadingDetails = other.m_loadingDetails;
}

Image::Image(Site *site, QMap<QString, QString> details, Profile *profile, Page* parent)
	: m_profile(profile), m_id(0), m_parentSite(site), m_extensionRotator(nullptr)
{
	m_settings = m_profile->getSettings();

	// Parents
	if (m_parentSite == nullptr)
	{
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
	m_fileSize = details.contains("file_size") ? details["file_size"].toInt() : 0;
	m_authorId = details.contains("creator_id") ? details["creator_id"].toInt() : 0;
	m_hasChildren = details.contains("has_children") && details["has_children"] == "true";
	m_hasNote = details.contains("has_note") && details["has_note"] == "true";
	m_hasComments = details.contains("has_comments") && details["has_comments"] == "true";
	m_fileUrl = details.contains("file_url") ? m_parentSite->fixUrl(details["file_url"]) : QUrl();
	m_sampleUrl = details.contains("sample_url") ? m_parentSite->fixUrl(details["sample_url"]) : QUrl();
	m_previewUrl = details.contains("preview_url") ? m_parentSite->fixUrl(details["preview_url"]) : QUrl();
	m_size = QSize(details.contains("width") ? details["width"].toInt() : 0, details.contains("height") ? details["height"].toInt() : 0);
	m_source = details.contains("source") ? details["source"] : "";

	// Page url
	if (details.contains("page_url"))
	{ m_pageUrl = details["page_url"]; }
	else
	{
		Api *api = m_parentSite->detailsApi();
		if (api != nullptr)
		{ m_pageUrl = api->detailsUrl(m_id, m_md5, m_parentSite).url; }
	}
	m_pageUrl = site->fixUrl(m_pageUrl).toString();

	// Rating
	setRating(details.contains("rating") ? details["rating"] : "");

	// Tags
	QStringList types = QStringList() << "general" << "artist" << "character" << "copyright" << "model" << "species" << "meta";
	for (const QString &typ : types)
	{
		const QString key = "tags_" + typ;
		if (!details.contains(key))
			continue;

		const TagType ttype(typ);
		QStringList t = details[key].split(' ', QString::SkipEmptyParts);
		for (QString tg : t)
		{
			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, ttype));
		}
	}
	if (m_tags.isEmpty() && details.contains("tags"))
	{
		QString tgs = QString(details["tags"]).replace(QRegularExpression("[\r\n\t]+"), " ");

		// Automatically find tag separator and split the list
		const int commas = tgs.count(", ");
		const int spaces = tgs.count(" ");
		const QStringList &t = commas >= 10 || (commas > 0 && (spaces - commas) / commas < 2)
			? tgs.split(", ", QString::SkipEmptyParts)
			: tgs.split(" ", QString::SkipEmptyParts);

		for (QString tg : t)
		{
			tg.replace("&amp;", "&");

			const int colon = tg.indexOf(':');
			if (colon != -1)
			{
				const QString tp = tg.left(colon).toLower();
				if (tp == "user")
				{ m_author = tg.mid(colon + 1); }
				else if (tp == "score")
				{ m_score = tg.midRef(colon + 1).toInt(); }
				else if (tp == "size")
				{
					QStringList size = tg.mid(colon + 1).split('x');
					if (size.size() == 2)
						m_size = QSize(size[0].toInt(), size[1].toInt());
				}
				else if (tp == "rating")
				{ setRating(tg.mid(colon + 1)); }
				else
				{ m_tags.append(Tag(tg)); }
			}
			else
			{ m_tags.append(Tag(tg)); }
		}
	}

	// Complete missing tag type information
	m_parentSite->tagDatabase()->load();
	QStringList unknownTags;
	for (Tag const &tag : qAsConst(m_tags))
		if (tag.type().isUnknown())
			unknownTags.append(tag.text());
	QMap<QString, TagType> dbTypes = m_parentSite->tagDatabase()->getTagTypes(unknownTags);
	for (Tag &tag : m_tags)
		if (dbTypes.contains(tag.text()))
			tag.setType(dbTypes[tag.text()]);

	// Get file url and try to improve it to save bandwidth
	m_url = m_fileUrl;
	const QString ext = getExtension(m_url);
	if (details.contains("ext") && !details["ext"].isEmpty())
	{
		const QString realExt = details["ext"];
		if (ext != realExt)
		{ setFileExtension(realExt); }
	}
	else if (ext == QLatin1String("jpg") && !m_previewUrl.isEmpty())
	{
		bool fixed = false;
		const QString previewExt = getExtension(QUrl(details["preview_url"]));
		if (!m_sampleUrl.isEmpty())
		{
			// Guess extension from sample url
			const QString sampleExt = getExtension(QUrl(details["sample_url"]));
			if (sampleExt != QLatin1String("jpg") && sampleExt != QLatin1String("png") && sampleExt != ext && previewExt == ext)
			{
				m_url = setExtension(m_url, sampleExt);
				fixed = true;
			}
		}

		// Guess the extension from the tags
		if (!fixed)
		{
			if ((hasTag(QStringLiteral("swf")) || hasTag(QStringLiteral("flash"))) && ext != QLatin1String("swf"))
			{ setFileExtension(QStringLiteral("swf")); }
			else if ((hasTag(QStringLiteral("gif")) || hasTag(QStringLiteral("animated_gif"))) && ext != QLatin1String("webm") && ext != QLatin1String("mp4"))
			{ setFileExtension(QStringLiteral("gif")); }
			else if (hasTag(QStringLiteral("mp4")) && ext != QLatin1String("gif") && ext != QLatin1String("webm"))
			{ setFileExtension(QStringLiteral("mp4")); }
			else if (hasTag(QStringLiteral("animated_png")) && ext != QLatin1String("webm") && ext != QLatin1String("mp4"))
			{ setFileExtension(QStringLiteral("png")); }
			else if ((hasTag(QStringLiteral("webm")) || hasTag(QStringLiteral("animated"))) && ext != QLatin1String("gif") && ext != QLatin1String("mp4"))
			{ setFileExtension(QStringLiteral("webm")); }
		}
	}
	else if (details.contains("image") && details["image"].contains("MB // gif\" height=\"") && ext != QLatin1String("gif"))
	{ m_url = setExtension(m_url, QStringLiteral("gif")); }

	// Remove ? in urls
	m_url = removeCacheUrl(m_url);
	m_fileUrl = removeCacheUrl(m_fileUrl);
	m_sampleUrl = removeCacheUrl(m_sampleUrl);
	m_previewUrl = removeCacheUrl(m_previewUrl);

	// We use the sample URL as the URL for zip files (ugoira) or if the setting is set
	const bool downloadOriginals = m_settings->value("Save/downloadoriginals", true).toBool();
	if (!m_sampleUrl.isEmpty() && (getExtension(m_url) == "zip" || !downloadOriginals))
		m_url = m_sampleUrl.toString();

	// Creation date
	m_createdAt = QDateTime();
	if (details.contains("created_at"))
	{ m_createdAt = qDateTimeFromString(details["created_at"]); }
	else if (details.contains("date"))
	{ m_createdAt = QDateTime::fromString(details["date"], Qt::ISODate); }

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

Image::~Image()
{
	if (!m_temporaryPath.isEmpty())
		QFile::remove(m_temporaryPath);
}

void Image::loadDetails(bool rateLimit)
{
	if (m_loadingDetails)
		return;

	if (m_loadedDetails || m_pageUrl.isEmpty())
	{
		emit finishedLoadingTags();
		return;
	}

	m_parentSite->getAsync(rateLimit ? Site::QueryType::Retry : Site::QueryType::Details, m_pageUrl, [this](QNetworkReply *reply) {
		if (m_loadDetails != nullptr)
			m_loadDetails->deleteLater();

		m_loadDetails = reply;
		m_loadDetails->setParent(this);
		m_loadingDetails = true;

		connect(m_loadDetails, &QNetworkReply::finished, this, &Image::parseDetails);
	});
}
void Image::abortTags()
{
	if (m_loadingDetails && m_loadDetails->isRunning())
	{
		m_loadDetails->abort();
		m_loadingDetails = false;
	}
}
void Image::parseDetails()
{
	m_loadingDetails = false;

	// Aborted
	if (m_loadDetails->error() == QNetworkReply::OperationCanceledError)
	{
		m_loadDetails->deleteLater();
		m_loadDetails = nullptr;
		return;
	}

	// Check redirection
	QUrl redir = m_loadDetails->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_pageUrl = redir;
		loadDetails();
		return;
	}

	const int statusCode = m_loadDetails->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 429)
	{
		log(QStringLiteral("Details limit reached (429). New try."));
		loadDetails(true);
		return;
	}

	const QString source = QString::fromUtf8(m_loadDetails->readAll());

	// Get an api able to parse details
	Api *api = m_parentSite->detailsApi();
	if (api == nullptr)
		return;

	// Parse source
	ParsedDetails ret = api->parseDetails(source, m_parentSite);
	if (!ret.error.isEmpty())
	{
		log(QStringLiteral("[%1][%2] %3").arg(m_parentSite->url(), api->getName(), ret.error), Logger::Warning);
		emit finishedLoadingTags();
		return;
	}

	// Fill data from parsing result
	if (!ret.pools.isEmpty())
	{ m_pools = ret.pools; }
	if (!ret.tags.isEmpty())
	{ m_tags = ret.tags; }
	if (ret.createdAt.isValid())
	{ m_createdAt = ret.createdAt; }

	// Image url
	if (!ret.imageUrl.isEmpty())
	{
		const QUrl before = m_url;

		QUrl newUrl = m_parentSite->fixUrl(ret.imageUrl, before);
		m_url = newUrl;
		m_fileUrl = newUrl;

		if (before != m_url)
		{
			delete m_extensionRotator;
			m_extensionRotator = nullptr;
			setFileSize(0);
			emit urlChanged(before, m_url);
		}
	}

	// Get rating from tags
	if (m_rating.isEmpty())
	{
		int ratingTagIndex = -1;
		for (int it = 0; it < m_tags.count(); ++it)
		{
			if (m_tags[it].type().name() == "rating")
			{
				m_rating = m_tags[it].text();
				ratingTagIndex = it;
				break;
			}
		}
		if (ratingTagIndex != -1)
		{ m_tags.removeAt(ratingTagIndex); }
	}

	m_loadDetails->deleteLater();
	m_loadDetails = nullptr;
	m_loadedDetails = true;

	refreshTokens();

	emit finishedLoadingTags();
}

QStringList Image::path(QString fn, QString pth, int counter, bool complex, bool simple, bool maxLength, bool shouldFixFilename, bool getFull) const
{
	if (!simple)
	{
		if (fn.isEmpty())
		{ fn = m_settings->value("Save/filename").toString(); }
		if (pth.isEmpty())
		{ pth = m_settings->value("Save/path").toString(); }
	}

	Filename filename(fn);
	return filename.path(*this, m_profile, pth, counter, complex, maxLength, shouldFixFilename, getFull);
}

/**
 * Try to guess the size of the image in pixels for sorting.
 * @return The guessed number of pixels in the image.
 */
int Image::value() const
{
	// Get from image size
	if (!m_size.isEmpty())
		return m_size.width() * m_size.height();

	// Get from tags
	if (hasTag("incredibly_absurdres"))
		return 10000 * 10000;
	else if (hasTag("absurdres"))
		return 3200 * 2400;
	else if (hasTag("highres"))
		return 1600 * 1200;
	else if (hasTag("lowres"))
		return 500 * 500;

	return 1200 * 900;
}

QStringList Image::stylishedTags(Profile *profile) const
{
	TagStylist stylist(profile);
	return stylist.stylished(m_tags);
}

Image::SaveResult Image::save(const QString &path, bool force, bool basic, bool addMd5, bool startCommands, int count, bool postSave)
{
	SaveResult res = SaveResult::Saved;

	QFile f(path);
	if (!f.exists() || force)
	{
		const QPair<QString, QString> md5action = m_profile->md5Action(md5());
		const QString &whatToDo = md5action.first;
		const QString &md5Duplicate = md5action.second;

		// Only create the destination directory if we're going to put a file there
		if (md5Duplicate.isEmpty() || force || whatToDo != "ignore")
		{
			const QString p = path.section(QDir::separator(), 0, -2);
			QDir pathToFile(p), dir;
			if (!pathToFile.exists() && !dir.mkpath(p))
			{
				log(QStringLiteral("Impossible to create the destination folder: %1.").arg(p), Logger::Error);
				return SaveResult::Error;
			}
		}

		if (md5Duplicate.isEmpty() || whatToDo == "save" || force)
		{
			if (!m_savePath.isEmpty() && QFile::exists(m_savePath))
			{
				log(QStringLiteral("Saving image in <a href=\"file:///%1\">%1</a> (from <a href=\"file:///%2\">%2</a>)").arg(path, m_savePath));
				QFile(m_savePath).copy(path);
			}
			else
			{
				if (m_data.isEmpty())
				{ return SaveResult::NotLoaded; }

				log(QStringLiteral("Saving image in <a href=\"file:///%1\">%1</a>").arg(path));

				if (f.open(QFile::WriteOnly))
				{
					if (f.write(m_data) < 0)
					{
						f.close();
						f.remove();
						log(QStringLiteral("File saving error: %1)").arg(f.errorString()), Logger::Error);
						return SaveResult::Error;
					}
					f.close();
				}
				else
				{
					log(QStringLiteral("Unable to open file"));
					return SaveResult::Error;
				}
			}
		}
		else if (whatToDo == "copy")
		{
			log(QStringLiteral("Copy from <a href=\"file:///%1\">%1</a> to <a href=\"file:///%2\">%2</a>").arg(md5Duplicate, path));
			QFile(md5Duplicate).copy(path);

			res = SaveResult::Copied;
		}
		else if (whatToDo == "move")
		{
			log(QStringLiteral("Moving from <a href=\"file:///%1\">%1</a> to <a href=\"file:///%2\">%2</a>").arg(md5Duplicate, path));
			QFile::rename(md5Duplicate, path);
			m_profile->setMd5(md5(), path);

			res = SaveResult::Moved;
		}
		else
		{
			log(QStringLiteral("MD5 \"%1\" of the image <a href=\"%2\">%2</a> already found in file <a href=\"file:///%3\">%3</a>").arg(md5(), url().toString(), md5Duplicate));
			return SaveResult::Ignored;
		}

		if (postSave)
		{ postSaving(path, addMd5 && res == SaveResult::Saved, startCommands, count, basic); }
	}
	else
	{ res = SaveResult::AlreadyExists; }

	return res;
}
void Image::postSaving(const QString &path, bool addMd5, bool startCommands, int count, bool basic)
{
	if (addMd5)
	{ m_profile->addMd5(md5(), path); }

	// Save info to a text file
	if (!basic)
	{
		auto logFiles = getExternalLogFiles(m_settings);
		for (auto it = logFiles.begin(); it != logFiles.end(); ++it)
		{
			auto logFile = it.value();
			const QString textfileFormat = logFile["content"].toString();
			QStringList cont = this->path(textfileFormat, "", count, true, true, false, false, false);
			if (!cont.isEmpty())
			{
				const int locationType = logFile["locationType"].toInt();
				QString contents = cont.first();

				// File path
				QString fileTagsPath;
				if (locationType == 0)
					fileTagsPath = this->path(logFile["filename"].toString(), logFile["path"].toString(), 0, true, false, true, true, true).first();
				else if (locationType == 1)
					fileTagsPath = logFile["uniquePath"].toString();
				else if (locationType == 2)
					fileTagsPath = path + logFile["suffix"].toString();

				// Replace some post-save tokens
				contents.replace("%path:nobackslash%", QDir::toNativeSeparators(path).replace("\\", "/"))
						.replace("%path%", QDir::toNativeSeparators(path));

				// Append to file if necessary
				QFile fileTags(fileTagsPath);
				const bool append = fileTags.exists();
				if (fileTags.open(QFile::WriteOnly | QFile::Append | QFile::Text))
				{
					if (append)
						fileTags.write("\n");
					fileTags.write(contents.toUtf8());
					fileTags.close();
				}
			}
		}
	}

	// Keep original date
	if (m_settings->value("Save/keepDate", true).toBool())
		setFileCreationDate(path, createdAt());

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

	setSavePath(path);
}
QMap<QString, Image::SaveResult> Image::save(const QStringList &paths, bool addMd5, bool startCommands, int count, bool force)
{
	QMap<QString, Image::SaveResult> res;
	for (const QString &path : paths)
		res.insert(path, save(path, force, false, addMd5, startCommands, count));
	return res;
}
QMap<QString, Image::SaveResult> Image::save(const QString &filename, const QString &path, bool addMd5, bool startCommands, int count)
{
	const QStringList paths = this->path(filename, path, count, true, false, true, true, true);
	return save(paths, addMd5, startCommands, count, false);
}

QList<Tag> Image::filteredTags(const QStringList &remove) const
{
	QList<Tag> tags;

	QRegExp reg;
	reg.setCaseSensitivity(Qt::CaseInsensitive);
	reg.setPatternSyntax(QRegExp::Wildcard);
	for (const Tag &tag : m_tags)
	{
		bool removed = false;
		for (const QString &rem : remove)
		{
			reg.setPattern(rem);
			if (reg.exactMatch(tag.text()))
			{
				removed = true;
				break;
			}
		}

		if (!removed)
			tags.append(tag);
	}

	return tags;
}


const QUrl		&Image::url() const			{ return m_url;				}
const QString	&Image::rating() const		{ return m_rating;			}
Site			*Image::parentSite() const	{ return m_parentSite;		}
const QList<Tag>	&Image::tags() const	{ return m_tags;			}
const QList<Pool>	&Image::pools() const	{ return m_pools;			}
qulonglong		Image::id() const			{ return m_id;				}
int				Image::fileSize() const		{ return m_fileSize;		}
int				Image::width() const		{ return m_size.width();	}
int				Image::height() const		{ return m_size.height();	}
const QDateTime	&Image::createdAt() const	{ return m_createdAt;		}
const QUrl		&Image::fileUrl() const		{ return m_fileUrl;			}
const QUrl		&Image::pageUrl() const		{ return m_pageUrl;			}
QSize			Image::size() const			{ return m_size;			}
QPixmap			Image::previewImage() const	{ return m_imagePreview;	}
const QPixmap	&Image::previewImage()		{ return m_imagePreview;	}
Page			*Image::page() const		{ return m_parent;			}
const QByteArray&Image::data() const		{ return m_data;			}
bool			Image::isGallery() const	{ return m_isGallery;		}
ExtensionRotator	*Image::extensionRotator() const	{ return m_extensionRotator;	}

void Image::setPreviewImage(const QPixmap &preview)
{ m_imagePreview = preview; }
void Image::setTemporaryPath(const QString &path)
{
	setSavePath(path);

	if (m_temporaryPath == path)
		return;

	if (!m_temporaryPath.isEmpty())
		QFile::remove(m_temporaryPath);

	m_temporaryPath = path;
}
void Image::setSavePath(const QString &path)
{
	if (path != m_savePath)
	{
		m_savePath = path;
		refreshTokens();
	}
}
QString Image::savePath() const
{ return m_savePath; }

bool Image::shouldDisplaySample() const
{
	const bool getOriginals = m_settings->value("Save/downloadoriginals", true).toBool();
	const bool viewSample = m_settings->value("Zoom/viewSamples", false).toBool();

	return !m_sampleUrl.isEmpty() && (!getOriginals || viewSample);
}
QUrl Image::getDisplayableUrl() const
{ return shouldDisplaySample() ? m_sampleUrl : m_url; }

QStringList Image::tagsString() const
{
	QStringList tags;
	tags.reserve(m_tags.count());
	for (const Tag &tag : m_tags)
		tags.append(tag.text());
	return tags;
}

void Image::setUrl(const QUrl &url)
{
	setFileSize(0);
	emit urlChanged(m_url, url);
	m_url = url;
	refreshTokens();
}
void Image::setSize(QSize size)		{ m_size = size; refreshTokens();		}
void Image::setFileSize(int size)	{ m_fileSize = size; refreshTokens();	}
void Image::setData(const QByteArray &data)
{
	m_data = data;

	// Detect file extension from data headers
	const bool headerDetection = m_settings->value("Save/headerDetection", true).toBool();
	if (headerDetection)
	{
		QString ext = getExtensionFromHeader(m_data.left(12));
		const QString currentExt = getExtension(m_url);
		if (!ext.isEmpty() && ext != currentExt)
		{
			log(QStringLiteral("Setting image extension from header: '%1' (was '%2').").arg(ext, currentExt), Logger::Info);
			setFileExtension(ext);
		}
	}

	// Set MD5 by hashing this data if we don't already have it
	if (m_md5.isEmpty())
	{
		m_md5 = QCryptographicHash::hash(m_data, QCryptographicHash::Md5).toHex();
		refreshTokens();
	}
}
void Image::setTags(const QList<Tag> &tags)
{
	m_tags = tags;
	refreshTokens();
}

QColor Image::color() const
{
	// Blacklisted
	QStringList detected = m_profile->getBlacklist().match(tokens(m_profile));
	if (!detected.isEmpty())
		return { 0, 0, 0 };

	// Favorited (except for exact favorite search)
	auto favorites = m_profile->getFavorites();
	for (const Tag &tag : m_tags)
		if (!m_parent->search().contains(tag.text()))
			for (const Favorite &fav : favorites)
				if (fav.getName() == tag.text())
				{ return { 255, 192, 203 }; }

	// Image with a parent
	if (m_parentId != 0)
		return { 204, 204, 0 };

	// Image with children
	if (m_hasChildren)
		return { 0, 255, 0 };

	// Pending image
	if (m_status == "pending")
		return { 0, 0, 255 };

	return {};
}

QString Image::tooltip() const
{
	if (m_isGallery)
		return QStringLiteral("%1%2")
			.arg(m_id == 0 ? " " : tr("<b>ID:</b> %1<br/>").arg(m_id))
			.arg(m_name.isEmpty() ? " " : tr("<b>Name:</b> %1<br/>").arg(m_name));

	double size = m_fileSize;
	const QString unit = getUnit(&size);

	return QStringLiteral("%1%2%3%4%5%6%7%8")
		.arg(m_tags.isEmpty() ? " " : tr("<b>Tags:</b> %1<br/><br/>").arg(stylishedTags(m_profile).join(" ")))
		.arg(m_id == 0 ? " " : tr("<b>ID:</b> %1<br/>").arg(m_id))
		.arg(m_rating.isEmpty() ? " " : tr("<b>Rating:</b> %1<br/>").arg(m_rating))
		.arg(m_hasScore ? tr("<b>Score:</b> %1<br/>").arg(m_score) : " ")
		.arg(m_author.isEmpty() ? " " : tr("<b>User:</b> %1<br/><br/>").arg(m_author))
		.arg(m_size.width() == 0 || m_size.height() == 0 ? " " : tr("<b>Size:</b> %1 x %2<br/>").arg(QString::number(m_size.width()), QString::number(m_size.height())))
		.arg(m_fileSize == 0 ? " " : tr("<b>Filesize:</b> %1 %2<br/>").arg(QString::number(size), unit))
		.arg(!m_createdAt.isValid() ? " " : tr("<b>Date:</b> %1").arg(m_createdAt.toString(tr("'the 'MM/dd/yyyy' at 'hh:mm"))));
}

QList<QStrP> Image::detailsData() const
{
	const QString unknown = tr("<i>Unknown</i>");
	const QString yes = tr("yes");
	const QString no = tr("no");

	return {
		QStrP(tr("Tags"), stylishedTags(m_profile).join(' ')),
		QStrP(),
		QStrP(tr("ID"), m_id != 0 ? QString::number(m_id) : unknown),
		QStrP(tr("MD5"), !m_md5.isEmpty() ? m_md5 : unknown),
		QStrP(tr("Rating"), !m_rating.isEmpty() ? m_rating : unknown),
		QStrP(tr("Score"), QString::number(m_score)),
		QStrP(tr("Author"), !m_author.isEmpty() ? m_author : unknown),
		QStrP(),
		QStrP(tr("Date"), m_createdAt.isValid() ? m_createdAt.toString(tr("'the' MM/dd/yyyy 'at' hh:mm")) : unknown),
		QStrP(tr("Size"), !m_size.isEmpty() ? QString::number(m_size.width())+"x"+QString::number(m_size.height()) : unknown),
		QStrP(tr("Filesize"), m_fileSize != 0 ? formatFilesize(m_fileSize) : unknown),
		QStrP(),
		QStrP(tr("Page"), !m_pageUrl.isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(m_pageUrl.toString()) : unknown),
		QStrP(tr("URL"), !m_fileUrl.isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(m_fileUrl.toString()) : unknown),
		QStrP(tr("Source"), !m_source.isEmpty() ? QString("<a href=\"%1\">%1</a>").arg(m_source) : unknown),
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
	// If we know the path to the image or its content but not its md5, we calculate it first
	if (m_md5.isEmpty() && (!m_savePath.isEmpty() || !m_data.isEmpty()))
	{
		QCryptographicHash hash(QCryptographicHash::Md5);

		// Calculate from image data
		if (!m_data.isEmpty())
		{
			hash.addData(m_data);
		}

		// Calculate from image path
		else
		{
			QFile f(m_savePath);
			f.open(QFile::ReadOnly);
			hash.addData(&f);
			f.close();
		}

		m_md5 = hash.result().toHex();
	}

	return m_md5;
}

bool Image::hasTag(QString tag) const
{
	tag = tag.trimmed();
	for (const Tag &t : m_tags)
		if (QString::compare(t.text(), tag, Qt::CaseInsensitive) == 0)
			return true;
	return false;
}
bool Image::hasAnyTag(const QStringList &tags) const
{
	for (const QString &tag : tags)
		if (this->hasTag(tag))
			return true;
	return false;
}
bool Image::hasAllTags(const QStringList &tags) const
{
	for (const QString &tag : tags)
		if (!this->hasTag(tag))
			return false;
	return true;
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

	if (ext == "gif" || ext == "apng")
		return ext;

	if (ext == "png" && (hasTag(QStringLiteral("animated")) || hasTag(QStringLiteral("animated_png"))))
		return QStringLiteral("apng");

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
	if (!filename.needExactTags(m_parentSite))
		return;

	QEventLoop loop;
	QObject::connect(this, &Image::finishedLoadingTags, &loop, &QEventLoop::quit);
	loadDetails();
	loop.exec();
}

QStringList Image::paths(const Filename &filename, const QString &folder, int count) const
{
	return path(filename.getFormat(), folder, count, true, false, true, true, true);
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
	tokens.insert("website", Token(m_parentSite->url(), ""));
	tokens.insert("websitename", Token(m_parentSite->name(), ""));
	tokens.insert("md5", Token(md5(), ""));
	tokens.insert("date", Token(m_createdAt, QDateTime()));
	tokens.insert("id", Token(m_id, 0));
	tokens.insert("rating", Token(m_rating, "unknown"));
	tokens.insert("score", Token(m_score, 0));
	tokens.insert("height", Token(m_size.height(), 0));
	tokens.insert("width", Token(m_size.width(), 0));
	tokens.insert("mpixels", Token(m_size.width() * m_size.height(), 0));
	tokens.insert("url_file", Token(m_url, ""));
	tokens.insert("url_sample", Token(m_sampleUrl.toString(), ""));
	tokens.insert("url_thumbnail", Token(m_previewUrl.toString(), ""));
	tokens.insert("url_page", Token(m_pageUrl.toString(), ""));
	tokens.insert("source", Token(m_source, ""));
	tokens.insert("filesize", Token(m_fileSize, 0));
	tokens.insert("author", Token(m_author, ""));
	tokens.insert("authorid", Token(m_authorId, 0));
	tokens.insert("parentid", Token(m_parentId, 0));

	// Flags
	tokens.insert("has_children", Token(m_hasChildren, false));
	tokens.insert("has_note", Token(m_hasNote, false));
	tokens.insert("has_comments", Token(m_hasComments, false));

	// Search
	for (int i = 0; i < m_search.size(); ++i)
	{ tokens.insert("search_" + QString::number(i + 1), Token(m_search[i], "")); }
	for (int i = m_search.size(); i < 10; ++i)
	{ tokens.insert("search_" + QString::number(i + 1), Token("", "")); }
	tokens.insert("search", Token(m_search.join(' '), ""));

	// Tags
	QMap<QString, QStringList> details;
	for (const Tag &tag : filteredTags(remove))
	{
		const QString &t = tag.text();

		details[ignore.contains(t, Qt::CaseInsensitive) ? "general" : tag.type().name()].append(t);
		details["alls"].append(t);
		details["alls_namespaces"].append(tag.type().name());

		QString underscored = QString(t);
		underscored.replace(' ', '_');
		details["allos"].append(underscored);
	}

	// Shorten copyrights
	if (settings->value("Save/copyright_useshorter", true).toBool())
	{
		QStringList copyrights;
		for (const QString &cop : details["copyright"])
		{
			bool found = false;
			for (int r = 0; r < copyrights.size(); ++r)
			{
				if (copyrights.at(r).left(cop.size()) == cop.left(copyrights.at(r).size()))
				{
					if (cop.size() < copyrights.at(r).size())
					{ copyrights[r] = cop; }
					found = true;
				}
			}
			if (!found)
			{ copyrights.append(cop); }
		}
		details["copyright"] = copyrights;
	}

	// Tags
	tokens.insert("general", Token(details["general"]));
	tokens.insert("artist", Token(details["artist"], "replaceAll", "anonymous", "multiple artists"));
	tokens.insert("copyright", Token(details["copyright"], "replaceAll", "misc", "crossover"));
	tokens.insert("character", Token(details["character"], "replaceAll", "unknown", "group"));
	tokens.insert("model", Token(details["model"], "replaceAll", "unknown", "multiple"));
	tokens.insert("species", Token(details["species"], "replaceAll", "unknown", "multiple"));
	tokens.insert("meta", Token(details["meta"], "replaceAll", "none", "multiple"));
	tokens.insert("allos", Token(details["allos"]));
	tokens.insert("allo", Token(details["allos"].join(' '), ""));
	tokens.insert("tags", Token(details["alls"]));
	tokens.insert("all", Token(details["alls"]));
	tokens.insert("all_namespaces", Token(details["alls_namespaces"]));

	// Extension
	QString ext = getExtension(m_url);
	if (settings->value("Save/noJpeg", true).toBool() && ext == "jpeg")
		ext = "jpg";
	tokens.insert("ext", Token(ext, "jpg"));
	tokens.insert("filetype", Token(ext, "jpg"));

	return tokens;
}

Image::SaveResult Image::preSave(const QString &path)
{
	return save(path, false, false, false, false, 1, false);
}

void Image::postSave(const QString &path, SaveResult res, bool addMd5, bool startCommands, int count)
{
	postSaving(path, addMd5 && res == SaveResult::Saved, startCommands, count);
}
