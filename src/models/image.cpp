#include <QtScript>
#include "page.h"
#include "image.h"
#include "functions.h"
#include "site.h"
#include "commands.h"
#include "filename.h"


QString removeCacheUrl(QString url)
{
	QString get = url.section('?', 1, 1);
	if (get.isEmpty())
		return url;

	// Only remove ?integer
	bool ok;
	get.toInt(&ok);
	if (ok)
		return url.section('?', 0, 0);

	return url;
}

Image::Image()
	: QObject(), m_profile(nullptr)
{ }

// TODO: clean up this mess
Image::Image(const Image &other)
	: QObject(other.parent())
{
	m_parent = other.m_parent;

	m_id = other.m_id;
	m_score = other.m_score;
	m_parentId = other.m_parentId;
	m_fileSize = other.m_fileSize;
	m_authorId = other.m_authorId;
	m_previewTry = other.m_previewTry;

	m_hasChildren = other.m_hasChildren;
	m_hasNote = other.m_hasNote;
	m_hasComments = other.m_hasComments;
	m_hasScore = other.m_hasScore;

	m_url = other.m_url;
	m_md5 = other.m_md5;
	m_author = other.m_author;
	m_status = other.m_status;
	m_rating = other.m_rating;
	m_source = other.m_source;
	m_site = other.m_site;
	m_filename = other.m_filename;
	m_folder = other.m_folder;
	m_savePath = other.m_savePath;

	m_pageUrl = other.m_pageUrl;
	m_fileUrl = other.m_fileUrl;
	m_sampleUrl = other.m_sampleUrl;
	m_previewUrl = other.m_previewUrl;

	m_size = other.m_size;
	m_imagePreview = other.m_imagePreview;
	m_createdAt = other.m_createdAt;
	m_data = other.m_data;

	m_loadPreview = other.m_loadPreview;
	m_loadDetails = other.m_loadDetails;
	m_loadImage = other.m_loadImage;

	m_tags = other.m_tags;
	m_pools = other.m_pools;
	m_timer = other.m_timer;
	m_profile = other.m_profile;
	m_settings = other.m_settings;
	m_search = other.m_search;
	m_parentSite = other.m_parentSite;
	m_details = other.m_details;

	m_loadingPreview = other.m_loadingPreview;
	m_loadingDetails = other.m_loadingDetails;
	m_loadingImage = other.m_loadingImage;
	m_tryingSample = other.m_tryingSample;
}

Image::Image(Site *site, QMap<QString, QString> details, Profile *profile, Page* parent)
	: m_profile(profile), m_parentSite(site)
{
	m_settings = m_profile->getSettings();

	// Parents
	m_site = parent != nullptr ? parent->website() : (details.contains("website") ? details["website"] : "");
	//m_parentSite = parent != nullptr ? parent->site() : (details.contains("site") ? (Site*)details["site"].toLongLong() : nullptr);
	if (m_parentSite == nullptr)
	{
		log("Image has nullptr parent, aborting creation.");
		return;
	}

	// Other details
	m_details = details;
	m_md5 = details.contains("md5") ? details["md5"] : "";
	m_author = details.contains("author") ? details["author"] : "";
	m_status = details.contains("status") ? details["status"] : "";
	m_filename = details.contains("filename") ? details["filename"] : "";
	m_folder = details.contains("folder") ? details["folder"] : "";
	m_search = parent != nullptr ? parent->search() : (details.contains("search") ? details["search"].split(' ') : QStringList());
	m_id = details.contains("id") ? details["id"].toInt() : 0;
	m_score = details.contains("score") ? details["score"].toInt() : 0;
	m_hasScore = details.contains("score");
	m_parentId = details.contains("parent_id") ? details["parent_id"].toInt() : 0;
	m_fileSize = details.contains("file_size") ? details["file_size"].toInt() : 0;
	m_authorId = details.contains("creator_id") ? details["creator_id"].toInt() : 0;
	m_hasChildren = details.contains("has_children") ? details["has_children"] == "true" : false;
	m_hasNote = details.contains("has_note") ? details["has_note"] == "true" : false;
	m_hasComments = details.contains("has_comments") ? details["has_comments"] == "true" : false;
	m_fileUrl = details.contains("file_url") ? m_parentSite->fixUrl(details["file_url"]) : QUrl();
	m_sampleUrl = details.contains("sample_url") ? m_parentSite->fixUrl(details["sample_url"]) : QUrl();
	m_previewUrl = details.contains("preview_url") ? m_parentSite->fixUrl(details["preview_url"]) : QUrl();
	m_size = QSize(details.contains("width") ? details["width"].toInt() : 0, details.contains("height") ? details["height"].toInt() : 0);
	m_source = details.contains("source") ? details["source"] : "";

	// Page URL
	if (!details.contains("page_url") || details["page_url"].isEmpty())
	{
		QString pageUrl = m_parentSite->value("Urls/Html/Post");
		QString t = m_search.join(" ");
		if (m_parentSite->contains("DefaultTag") && t.isEmpty())
		{ t = m_parentSite->value("DefaultTag"); }
		pageUrl.replace("{tags}", QUrl::toPercentEncoding(t));
		pageUrl.replace("{id}", QString::number(m_id));
		m_pageUrl = m_parentSite->fixUrl(pageUrl);
	}
	else
	{ m_pageUrl = m_parentSite->fixUrl(details["page_url"]); }

	// Rating
	setRating(details.contains("rating") ? details["rating"] : "");

	// Tags
	if (details.contains("tags_general"))
	{
		QStringList t = details["tags_general"].split(" ");
		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			if (tg.isEmpty())
				continue;

			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, "general"));
		}
		t = details["tags_artist"].split(" ");
		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			if (tg.isEmpty())
				continue;

			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, "artist"));
		}
		t = details["tags_character"].split(" ");
		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			if (tg.isEmpty())
				continue;

			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, "character"));
		}
		t = details["tags_copyright"].split(" ");
		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			if (tg.isEmpty())
				continue;

			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, "copyright"));
		}
		t = details["tags_model"].split(" ");
		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			if (tg.isEmpty())
				continue;

			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, "model"));
		}
	}
	else if (details.contains("tags"))
	{
		// Automatically find tag separator and split the list
		QStringList t;
		if (details["tags"].count(", ") != 0 && details["tags"].count(" ") / details["tags"].count(", ") < 2)
		{ t = details["tags"].split(", "); }
		else
		{ t = details["tags"].split(" "); }

		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			if (tg.isEmpty())
				continue;

			tg.replace("&amp;", "&");

			int colon = tg.indexOf(':');
			if (colon != -1)
			{
				QString tp = tg.left(colon).toLower();
				if (tp == "user")
				{ m_author = tg.mid(colon + 1); }
				else if (tp == "score")
				{ m_score = tg.mid(colon + 1).toInt(); }
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

	// Get file url and try to improve it to save bandwidth
	m_url = details.contains("file_url") ? m_parentSite->fixUrl(details["file_url"]).toString() : "";
	QString ext = getExtension(m_url);
	if (m_details.contains("ext"))
	{
		QString realExt = m_details["ext"];
		if (ext != realExt)
		{ setFileExtension(realExt); }
	}
	else if (ext == "jpg" && !m_previewUrl.isEmpty())
	{
		bool fixed = false;
		QString previewExt = getExtension(details["preview_url"]);
		if (!m_sampleUrl.isEmpty())
		{
			// Guess extension from sample url
			QString sampleExt = getExtension(details["sample_url"]);
			if (sampleExt != "jpg" && sampleExt != "png" && sampleExt != ext && previewExt == ext)
			{
				m_url = setExtension(m_url, sampleExt);
				fixed = true;
			}
		}

		// Guess the extension from the tags
		if (!fixed)
		{
			if ((hasTag("swf") || hasTag("flash")) && ext != "swf")
			{ setFileExtension("swf"); }
			else if ((hasTag("gif") || hasTag("animated_gif")) && ext != "webm" && ext != "mp4")
			{ setFileExtension("gif"); }
			else if (hasTag("mp4") && ext != "gif" && ext != "webm")
			{ setFileExtension("mp4"); }
			else if (hasTag("animated_png") && ext != "webm" && ext != "mp4")
			{ setFileExtension("png"); }
			else if ((hasTag("webm") || hasTag("animated")) && ext != "gif" && ext != "mp4")
			{ setFileExtension("webm"); }
		}
	}
	else if (details.contains("image") && details["image"].contains("MB // gif\" height=\"") && !m_url.endsWith(".gif", Qt::CaseInsensitive))
	{ m_url = setExtension(m_url, "gif"); }

	// Remove ? in urls
	m_url = removeCacheUrl(m_url);
	m_fileUrl = removeCacheUrl(m_fileUrl.toString());
	m_sampleUrl = removeCacheUrl(m_sampleUrl.toString());
	m_previewUrl = removeCacheUrl(m_previewUrl.toString());

	// Creation date
	m_createdAt = QDateTime();
	if (details.contains("created_at"))
	{
		if (details["created_at"].toInt() != 0)
		{ m_createdAt.setTime_t(details["created_at"].toInt()); }
		else
		{ m_createdAt = qDateTimeFromString(details["created_at"]); }
	}
	else if (details.contains("date"))
	{ m_createdAt = QDateTime::fromString(details["date"], Qt::ISODate); }

	// Tech details
	m_parent = parent;
	m_previewTry = 0;
	m_loadPreview = nullptr;
	m_loadDetails = nullptr;
	m_loadImage = nullptr;
	m_loadingPreview = false;
	m_loadingDetails = false;
	m_loadingImage = false;
	m_tryingSample = false;
	m_pools = QList<Pool>();
}
Image::~Image()
{ }

void Image::loadPreview()
{
	if (m_previewUrl.isEmpty())
	{
		log(tr("Chargement de miniature annulé (miniature vide)."));
		return;
	}

	m_previewTry++;
	m_loadPreview = m_parentSite->get(m_parentSite->fixUrl(m_previewUrl), m_parent, "preview");
	m_loadPreview->setParent(this);
	m_loadingPreview = true;

	connect(m_loadPreview, SIGNAL(finished()), this, SLOT(parsePreview()));
}
void Image::abortPreview()
{
	if (m_loadingPreview && m_loadPreview->isRunning())
	{ m_loadPreview->abort(); }
}
void Image::parsePreview()
{
	m_loadingPreview = false;

	// Aborted
	if (m_loadPreview->error() == QNetworkReply::OperationCanceledError)
	{
		m_loadPreview->deleteLater();
		return;
	}

	// Check redirection
	QUrl redir = m_loadPreview->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_previewUrl = redir;
		loadPreview();
		return;
	}

	// Loading error
	if (m_loadPreview->error() != QNetworkReply::NoError)
	{
		log(tr("<b>Erreur :</b> %1").arg(tr("erreur de chargement de la miniature (%1)").arg(m_loadPreview->errorString())));
	}

	// Load preview from raw result
	QByteArray data = m_loadPreview->readAll();
	m_imagePreview.loadFromData(data);
	m_loadPreview->deleteLater();
	m_loadPreview = nullptr;

	// If nothing has been received
	if (m_imagePreview.isNull() && m_previewTry <= 3)
	{
		log(tr("<b>Attention :</b> %1").arg(tr("une des miniatures est vide (<a href=\"%1\">%1</a>). Nouvel essai (%2/%3)...").arg(m_previewUrl.toString()).arg(m_previewTry).arg(3)));

		if (hasTag("flash"))
		{ m_imagePreview.load(":/images/flash.png"); }
		else
		{
			loadPreview();
			return;
		}
	}

	emit finishedLoadingPreview();
}

void Image::loadDetails()
{
	m_loadDetails = m_parentSite->get(m_parentSite->fixUrl(m_pageUrl));
	m_loadDetails->setParent(this);
	m_loadingDetails = true;

	connect(m_loadDetails, SIGNAL(finished()), this, SLOT(parseDetails()));
}
void Image::abortTags()
{
	if (m_loadingDetails && m_loadDetails->isRunning())
	{ m_loadDetails->abort(); }
}
void Image::parseDetails()
{
	m_loadingDetails = false;

	// Aborted
	if (m_loadDetails->error() == QNetworkReply::OperationCanceledError)
	{
		m_loadDetails->deleteLater();
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

	QString source = QString::fromUtf8(m_loadDetails->readAll());

	// Pools
	if (m_parentSite->contains("Regex/Pools"))
	{
		m_pools.clear();
		QRegExp rx(m_parentSite->value("Regex/Pools"));
		rx.setMinimal(true);
		int pos = 0;
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QString previous = rx.cap(1), id = rx.cap(2), name = rx.cap(3), next = rx.cap(4);
			m_pools.append(Pool(id.toInt(), name, m_id, next.toInt(), previous.toInt()));
		}
	}

	// Tags
	QString rxtags;
	if (m_parentSite->contains("Regex/ImageTags"))
	{ rxtags = m_parentSite->value("Regex/ImageTags"); }
	else if (m_parentSite->contains("Regex/Tags"))
	{ rxtags = m_parentSite->value("Regex/Tags"); }
	if (!rxtags.isEmpty())
	{
		QStringList order = m_parentSite->value("Regex/TagsOrder").split('|', QString::SkipEmptyParts);
		QList<Tag> tgs = Tag::FromRegexp(rxtags, order, source);
		if (!tgs.isEmpty())
		{ m_tags = tgs; }
	}

	// Image url
	if ((m_url.isEmpty() || m_parentSite->contains("Regex/ForceImageUrl")) && m_parentSite->contains("Regex/ImageUrl"))
	{
		QRegExp rx = QRegExp(m_parentSite->value("Regex/ImageUrl"));
		rx.setMinimal(true);
		int pos = 0;
		QString before = m_url;
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QString newurl = m_parentSite->fixUrl(rx.cap(1), QUrl(m_url)).toString();
			m_url = newurl;
			m_fileUrl = newurl;
		}
		if (before != m_url)
		{
			setFileSize(0);
			emit urlChanged(before, m_url);
		}
	}

	m_loadDetails->deleteLater();
	m_loadDetails = nullptr;

	emit finishedLoadingTags();
}

int toDate(QString text)
{
	QDateTime date = QDateTime::fromString(text, "yyyy-MM-dd");
	if (date.isValid())
	{ return date.toString("yyyyMMdd").toInt(); }
	date = QDateTime::fromString(text, "MM/dd/yyyy");
	if (date.isValid())
	{ return date.toString("yyyyMMdd").toInt(); }
	return 0;
}

QString Image::match(QString filter, bool invert) const
{
	QStringList mathematicaltypes = QStringList() << "id" << "width" << "height" << "score" << "mpixels" << "filesize" << "date";
	QStringList types = QStringList() << "rating" << "source" << mathematicaltypes;

	// Invert the filter by prepending '-'
	if (filter.startsWith('-'))
	{
		filter = filter.right(filter.length()-1);
		invert = !invert;
	}

	// Metatags
	if (filter.contains(":"))
	{
		QString type = filter.section(':', 0, 0).toLower();
		filter = filter.section(':', 1).toLower();
		if (!types.contains(type))
		{ return QObject::tr("unknown type \"%1\" (available types: \"%2\")").arg(type, types.join("\", \"")); }
		if (mathematicaltypes.contains(type))
		{
			int input = 0;
			if (type == "id")		{ input = m_id;										}
			if (type == "width")	{ input = m_size.width();							}
			if (type == "height")	{ input = m_size.height();							}
			if (type == "score")	{ input = m_score;									}
			if (type == "mpixels")	{ input = m_size.width() * m_size.height();			}
			if (type == "filesize")	{ input = m_fileSize;								}
			if (type == "date")		{ input = m_createdAt.toString("yyyyMMdd").toInt();	}

			bool cond;
			if (type == "date")
			{
				if (filter.startsWith("..") || filter.startsWith("<="))
				{ cond = input <= toDate(filter.right(filter.size()-2)); }
				else if (filter.endsWith(".."))
				{ cond = input >= toDate(filter.left(filter.size()-2)); }
				else if (filter.startsWith(">="))
				{ cond = input >= toDate(filter.right(filter.size()-2)); }
				else if (filter.startsWith("<"))
				{ cond = input < toDate(filter.right(filter.size()-1)); }
				else if (filter.startsWith(">"))
				{ cond = input > toDate(filter.right(filter.size()-1)); }
				else if (filter.contains(".."))
				{ cond = input >= toDate(filter.left(filter.indexOf(".."))) && input <= toDate(filter.right(filter.size()-filter.indexOf("..")-2));	}
				else
				{ cond = input == toDate(filter); }
			}
			else
			{
				if (filter.startsWith("..") || filter.startsWith("<="))
				{ cond = input <= filter.right(filter.size()-2).toInt(); }
				else if (filter.endsWith(".."))
				{ cond = input >= filter.left(filter.size()-2).toInt(); }
				else if (filter.startsWith(">="))
				{ cond = input >= filter.right(filter.size()-2).toInt(); }
				else if (filter.startsWith("<"))
				{ cond = input < filter.right(filter.size()-1).toInt(); }
				else if (filter.startsWith(">"))
				{ cond = input > filter.right(filter.size()-1).toInt(); }
				else if (filter.contains(".."))
				{ cond = input >= filter.left(filter.indexOf("..")).toInt() && input <= filter.right(filter.size()-filter.indexOf("..")-2).toInt();	}
				else
				{ cond = input == filter.toInt(); }
			}

			if (!cond && !invert)
			{ return QObject::tr("image's %1 does not match").arg(type); }
			if (cond && invert)
			{ return QObject::tr("image's %1 match").arg(type); }
		}
		else
		{
			if (type == "rating")
			{
				QMap<QString,QString> assoc;
				assoc["s"] = "safe";
				assoc["q"] = "questionable";
				assoc["e"] = "explicit";

				if (assoc.contains(filter))
					filter = assoc[filter];

				bool cond = m_rating.toLower().startsWith(filter.left(1));
				if (!cond && !invert)
				{ return QObject::tr("image is not \"%1\"").arg(filter); }
				if (cond && invert)
				{ return QObject::tr("image is \"%1\"").arg(filter); }
			}
			else if (type == "source")
			{
				QRegExp rx = QRegExp(filter+"*", Qt::CaseInsensitive, QRegExp::Wildcard);
				bool cond = rx.exactMatch(m_source);
				if (!cond && !invert)
				{ return QObject::tr("image's source does not starts with \"%1\"").arg(filter); }
				if (cond && invert)
				{ return QObject::tr("image's source starts with \"%1\"").arg(filter); }
			}
		}
	}
	else if (!filter.isEmpty())
	{
		// Check if any tag match the filter (case insensitive plain text with wildcards allowed)
		bool cond = false;
		for (Tag tag : m_tags)
		{
			QRegExp reg;
			reg.setCaseSensitivity(Qt::CaseInsensitive);
			reg.setPatternSyntax(QRegExp::Wildcard);
			reg.setPattern(filter.trimmed());
			if (reg.exactMatch(tag.text()))
			{
				cond = true;
				break;
			}
		}

		if (!cond && !invert)
		{ return QObject::tr("image does not contains \"%1\"").arg(filter); }
		if (cond && invert)
		{ return QObject::tr("image contains \"%1\"").arg(filter); }
	}

	return QString();
}

QStringList Image::filter(QStringList filters) const
{
	QStringList ret;
	for (QString filter : filters)
	{
		QString match = this->match(filter);
		if (!match.isEmpty())
			ret.append(match);
	}
	return ret;
}

/**
 * Return the filename of the image according to the user's settings.
 * @param fn The user's filename.
 * @param pth The user's root save path.
 * @param counter Current image count (used for batch downloads).
 * @param complex Whether the filename is complex or not (contains conditionals).
 * @param simple True to force using the fn and pth parameters.
 * @return The filename of the image, with any token replaced.
 */
QStringList Image::path(QString fn, QString pth, int counter, bool complex, bool simple, bool maxlength, bool shouldFixFilename, bool getFull) const
{
	if (!simple)
	{
		if (!m_filename.isEmpty())
		{ fn = m_filename; }
		if (!m_folder.isEmpty())
		{ pth = m_folder; }

		if (fn.isEmpty())
		{ fn = m_settings->value("Save/filename").toString(); }
		if (pth.isEmpty())
		{ pth = m_settings->value("Save/path").toString(); }
	}

	Filename filename(fn);
	return filename.path(*this, m_profile, pth, counter, complex, maxlength, shouldFixFilename, getFull);
}

void Image::loadImage()
{
	if (m_loadImage != nullptr)
		m_loadImage->deleteLater();

	m_loadImage = m_parentSite->get(m_parentSite->fixUrl(m_url), m_parent, "image", this);
	m_loadImage->setParent(this);
	//m_timer.start();
	m_loadingImage = true;

	connect(m_loadImage, &QNetworkReply::downloadProgress, this, &Image::downloadProgressImageS);
	connect(m_loadImage, &QNetworkReply::finished, this, &Image::finishedImageS);
}
void Image::finishedImageS()
{
	m_loadingImage = false;

	// Aborted
	if (m_loadImage->error() == QNetworkReply::OperationCanceledError)
	{
		m_loadImage->deleteLater();
		return;
	}

	QUrl redir = m_loadImage->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_loadImage->deleteLater();
		m_url = redir.toString();
		loadImage();
		return;
	}

	if (m_loadImage->error() == QNetworkReply::ContentNotFoundError)
	{
		bool sampleFallback = m_settings->value("Save/samplefallback", true).toBool();
		QString ext = getExtension(m_url);
		bool animated = hasTag("gif") || hasTag("animated_gif") || hasTag("mp4") || hasTag("animated_png") || hasTag("webm") || hasTag("animated");
		bool isLast = (animated && ext == "swf") || ext == "mp4";

		if (!isLast || (sampleFallback && !m_sampleUrl.isEmpty() && !m_tryingSample))
		{
			if (isLast)
			{
				setUrl(m_sampleUrl.toString());
				m_tryingSample = true;
				log(tr("Image non trouvée. Nouvel essai avec son sample..."));
			}
			else
			{
				QMap<QString,QString> nextext;
				if (animated)
				{
					nextext["webm"] = "mp4";
					nextext["mp4"] = "gif";
					nextext["gif"] = "jpg";
					nextext["jpg"] = "png";
					nextext["png"] = "jpeg";
					nextext["jpeg"] = "swf";
				}
				else
				{
					nextext["jpg"] = "png";
					nextext["png"] = "gif";
					nextext["gif"] = "jpeg";
					nextext["jpeg"] = "webm";
					nextext["webm"] = "swf";
					nextext["swf"] = "mp4";
				}

				QString oldUrl = m_url;
				QString newext = nextext.contains(ext) ? nextext[ext] : "jpg";
				m_url = setExtension(m_url, newext);

				log(tr("Image non trouvée (%1). Nouvel essai avec l'extension %2...").arg(oldUrl, newext));
			}

			loadImage();
			return;
		}
		else
		{
			log(tr("Image non trouvée."));
		}
	}
	else
	{
		m_data = m_loadImage->readAll();
	}

	emit finishedImage();
}
void Image::downloadProgressImageS(qint64 v1, qint64 v2)
{
	if (m_loadImage != nullptr && v2 > 0/* && (v1 == v2 || m_timer.elapsed() > 500)*/)
	{
		//m_timer.restart();
		emit downloadProgressImage(v1, v2);
	}
}
void Image::abortImage()
{
	if (m_loadingImage && m_loadImage->isRunning())
	{ m_loadImage->abort(); }
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

/**
 * Checks whether an image contains a given set of tags.
 *
 * @param blacklistedtags The list of tags to check.
 * @return The blacklisted tags found in the image (empty list if none is found).
 */
QStringList Image::blacklisted(QStringList blacklistedtags, bool invert) const
{
	QStringList detected;
	QRegExp reg;
	reg.setCaseSensitivity(Qt::CaseInsensitive);
	reg.setPatternSyntax(QRegExp::Wildcard);
	for (QString tag : blacklistedtags)
	{
		if (!this->match(tag, invert).isEmpty())
		{ detected.append(tag); }
	}
	return detected;
}

QStringList Image::stylishedTags(Profile *profile) const
{
	QStringList ignored = profile->getIgnored();
	QStringList blacklisted = profile->getSettings()->value("blacklistedtags").toString().split(' ');

	QStringList t;
	for (Tag tag : m_tags)
		t.append(tag.stylished(profile, ignored, blacklisted));

	t.sort();
	return t;
}

Image::SaveResult Image::save(QString path, bool force, bool basic)
{
	SaveResult res = SaveResult::Saved;

	QFile f(path);
	if (!f.exists() || force)
	{
		QDir path_to_file(path.section(QDir::toNativeSeparators("/"), 0, -2));
		if (!path_to_file.exists())
		{
			QDir dir;
			if (!dir.mkpath(path.section(QDir::toNativeSeparators("/"), 0, -2)))
				return SaveResult::Error;
		}

		QString whatToDo = m_settings->value("Save/md5Duplicates", "save").toString();
		QString md5Duplicate = m_profile->md5Exists(md5());
		if (md5Duplicate.isEmpty() || whatToDo == "save" || force)
		{
			log(tr("Sauvegarde de l'image dans le fichier <a href=\"file:///%1\">%1</a>").arg(path));
			if (!m_source.isEmpty() && QFile::exists(m_source))
			{ QFile::copy(m_source, path); }
			else
			{
				m_profile->addMd5(md5(), path);

				if (f.open(QFile::WriteOnly))
				{
					f.write(m_data);
					f.close();
				}
				else
				{ log(tr("Impossible d'ouvrir le fichier")); }
			}

			if (m_settings->value("Textfile/activate", false).toBool() && !basic)
			{
				QStringList cont = this->path(m_settings->value("Textfile/content", "%all%").toString(), "", 1, true, true, false, false);
				if (!cont.isEmpty())
				{
					QString contents = cont.at(0);
					QFile file_tags(path + ".txt");
					if (file_tags.open(QFile::WriteOnly | QFile::Text))
					{
						file_tags.write(contents.toUtf8());
						file_tags.close();
					}
				}
			}
			if (m_settings->value("SaveLog/activate", false).toBool() && !m_settings->value("SaveLog/file", "").toString().isEmpty() && !basic)
			{
				QStringList cont = this->path(m_settings->value("SaveLog/format", "%website% - %md5% - %all%").toString(), "", 1, true, true, false, false);
				if (!cont.isEmpty())
				{
					QString contents = cont.at(0);
					QFile file_tags(m_settings->value("SaveLog/file", "").toString());
					if (file_tags.open(QFile::WriteOnly | QFile::Append | QFile::Text))
					{
						file_tags.write(contents.toUtf8() + "\n");
						file_tags.close();
					}
				}
			}
		}
		else if (whatToDo == "copy")
		{
			log(tr("Copie depuis <a href=\"file:///%1\">%1</a> vers <a href=\"file:///%2\">%2</a>").arg(md5Duplicate).arg(path));
			QFile::copy(md5Duplicate, path);

			res = SaveResult::Copied;
		}
		else if (whatToDo == "move")
		{
			log(tr("Déplacement depuis <a href=\"file:///%1\">%1</a> vers <a href=\"file:///%2\">%2</a>").arg(md5Duplicate).arg(path));
			QFile::rename(md5Duplicate, path);
			m_profile->setMd5(md5(), path);

			res = SaveResult::Moved;
		}
		else
		{ return SaveResult::Ignored; }

		// Keep original date
		if (m_settings->value("Save/keepDate", true).toBool())
			setFileCreationDate(path, createdAt());

		// Commands
		Commands &commands = m_profile->getCommands();
		commands.before();
			for (Tag tag : tags())
			{ commands.tag(*this, tag, false); }
			commands.image(*this, path);
			for (Tag tag : tags())
			{ commands.tag(*this, tag, true); }
		commands.after();
	}
	else
	{ res = SaveResult::AlreadyExists; }

	return res;
}
QMap<QString, Image::SaveResult> Image::save(QStringList paths)
{
	QMap<QString, Image::SaveResult> res;
	for (QString path : paths)
		res.insert(path, save(path));
	return res;
}
QMap<QString, Image::SaveResult> Image::save(QString filename, QString path)
{
	QStringList paths = this->path(filename, path, 0, true, false, true, true, true);
	return save(paths);
}

QList<Tag> Image::filteredTags(QStringList remove) const
{
	QList<Tag> tags;

	QRegExp reg;
	reg.setCaseSensitivity(Qt::CaseInsensitive);
	reg.setPatternSyntax(QRegExp::Wildcard);
	for (Tag tag : m_tags)
	{
		bool removed = false;
		for (QString rem : remove)
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


QString			Image::url() const			{ return m_url;				}
QString			Image::author() const		{ return m_author;			}
QString			Image::status() const		{ return m_status;			}
QString			Image::rating() const		{ return m_rating;			}
QString			Image::source() const		{ return m_source;			}
QString			Image::site() const			{ return m_site;			}
Site			*Image::parentSite() const	{ return m_parentSite;		}
QString			Image::filename() const		{ return m_filename;		}
QString			Image::folder() const		{ return m_folder;			}
QList<Tag>		Image::tags() const			{ return m_tags;			}
QList<Pool>		Image::pools() const		{ return m_pools;			}
int				Image::id() const			{ return m_id;				}
int				Image::score() const		{ return m_score;			}
int				Image::parentId() const		{ return m_parentId;		}
int				Image::fileSize() const		{ return m_fileSize;		}
int				Image::width() const		{ return m_size.width();	}
int				Image::height() const		{ return m_size.height();	}
int				Image::authorId() const		{ return m_authorId;		}
QDateTime		Image::createdAt() const	{ return m_createdAt;		}
bool			Image::hasChildren() const	{ return m_hasChildren;		}
bool			Image::hasNote() const		{ return m_hasNote;			}
bool			Image::hasComments() const	{ return m_hasComments;		}
bool			Image::hasScore() const		{ return m_hasScore;		}
QUrl			Image::fileUrl() const		{ return m_fileUrl;			}
QUrl			Image::sampleUrl() const	{ return m_sampleUrl;		}
QUrl			Image::previewUrl() const	{ return m_previewUrl;		}
QUrl			Image::pageUrl() const		{ return m_pageUrl;			}
QSize			Image::size() const			{ return m_size;			}
QPixmap			Image::previewImage() const	{ return m_imagePreview;	}
Page			*Image::page() const		{ return m_parent;			}
QByteArray		Image::data() const			{ return m_data;			}
QNetworkReply	*Image::imageReply() const	{ return m_loadImage;		}
QNetworkReply	*Image::tagsReply() const	{ return m_loadDetails;		}
QSettings		*Image::settings() const	{ return m_settings;		}
QMap<QString,QString> Image::details() const{ return m_details;			}
QStringList		Image::search() const		{ return m_search;			}

QStringList Image::tagsString() const
{
	QStringList tags;
	for (Tag tag : m_tags)
	{ tags.append(tag.text()); }
	return tags;
}

void	Image::setUrl(QString u)
{
	setFileSize(0);
	emit urlChanged(m_url, u);
	m_url = u;
}
void	Image::setFileSize(int s)		{ m_fileSize = s;			}
void	Image::setData(QByteArray d)
{
	m_data = d;

	// Set MD5 by hashing this data if we don't already have it
	if (m_md5.isEmpty())
	{
		m_md5 = QCryptographicHash::hash(m_data, QCryptographicHash::Md5).toHex();
	}
}
void Image::setSavePath(QString savePath)
{
	m_savePath = savePath;
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
	for (Tag t : m_tags)
		if (QString::compare(t.text(), tag, Qt::CaseInsensitive) == 0)
			return true;
	return false;
}
bool Image::hasAnyTag(QStringList tags) const
{
	for (QString tag : tags)
		if (this->hasTag(tag))
			return true;
	return false;
}
bool Image::hasAllTags(QStringList tags) const
{
	for (QString tag : tags)
		if (!this->hasTag(tag))
			return false;
	return true;
}

void Image::unload()
{
	m_data.clear();
}

void Image::setRating(QString rating)
{
	QMap<QString,QString> assoc;
		assoc["s"] = "safe";
		assoc["q"] = "questionable";
		assoc["e"] = "explicit";

	if (assoc.contains(rating))
	{ m_rating = assoc.value(rating); }
	else
	{ m_rating = rating.toLower(); }
}

void Image::setFileExtension(QString ext)
{
	m_url = setExtension(m_url, ext);
	m_fileUrl = setExtension(m_fileUrl.toString(), ext);
}
