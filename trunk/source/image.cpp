#include "image.h"
#include "functions.h"



Image::Image(QMap<QString, QString> details, int timezonedecay, Page* parent)
{
	m_site = details.contains("site") ? details["site"] : m_url.section("/", 2, 2);
	m_url = details.contains("file_url") ? details["file_url"] : "";
	m_md5 = details.contains("md5") ? details["md5"] : "";
	m_author = details.contains("author") ? details["author"] : "";
	m_status = details.contains("status") ? details["status"] : "";
	m_rating = details.contains("rating") ? details["rating"] : "";
	QStringMap assoc;
		assoc["s"] = tr("Safe");
		assoc["q"] = tr("Questionable");
		assoc["e"] = tr("Explicit");
	if (assoc.contains(m_rating))
	{ m_rating = assoc[m_rating]; }
	m_source = details.contains("source") ? details["source"] : "";
	m_tags = QList<Tag*>();
	if (details.contains("tags"))
	{
		QStringList t = details["tags"].split(" ");
		for (int i = 0; i < t.count(); i++)
		{ m_tags.append(new Tag(t.at(i))); }
	}
	m_id = details.contains("id") ? details["id"].toInt() : 0;
	m_score = details.contains("score") ? details["score"].toInt() : 0;
	m_parentId = details.contains("parent_id") ? details["parent_id"].toInt() : 0;
	m_fileSize = details.contains("file_size") ? details["file_size"].toInt() : 0;
	m_authorId = details.contains("creator_id") ? details["creator_id"].toInt() : 0;
	m_createdAt = QDateTime();
	if (details.contains("created_at"))
	{
		if (details["created_at"].toInt() != 0)
		{ m_createdAt.setTime_t(details["created_at"].toInt()); }
		else
		{ m_createdAt = qDateTimeFromString(details["created_at"], timezonedecay); }
	}
	m_hasChildren = details.contains("has_children") ? details["has_children"] == "true" : false;
	m_hasNote = details.contains("has_note") ? details["has_note"] == "true" : false;
	m_hasComments = details.contains("has_comments") ? details["has_comments"] == "true" : false;
	m_pageUrl = details.contains("page_url") ? QUrl(details["page_url"]) : QUrl();
	m_fileUrl = details.contains("file_url") ? QUrl(details["file_url"]) : QUrl();
	m_sampleUrl = details.contains("sample_url") ? QUrl(details["sample_url"]) : QUrl();
	m_previewUrl = details.contains("preview_url") ? QUrl(details["preview_url"]) : QUrl();
	m_size = QSize(details.contains("width") ? details["width"].toInt() : 0, details.contains("height") ? details["height"].toInt() : 0);
	m_parent = parent;

	m_loadPreviewExists = false;
	m_loadTagsExists = false;
}
Image::~Image()
{ delete &m_imagePreview; }

void Image::loadPreview()
{
	m_loadPreviewExists = true;
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parsePreview(QNetworkReply*)));
	m_loadPreview = manager->get(QNetworkRequest(m_previewUrl));
}
void Image::abortPreview()
{
	if (m_loadPreviewExists)
	{
		if (m_loadPreview->isRunning())
		{ m_loadPreview->abort(); }
	}
}
void Image::loadTags()
{
	m_loadPreviewExists = true;
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseTags(QNetworkReply*)));
	m_loadTags = manager->get(QNetworkRequest(m_pageUrl));
}
void Image::abortTags()
{
	if (m_loadTags->isRunning())
	{ m_loadTags->abort(); }
}

void Image::parsePreview(QNetworkReply* r)
{
	m_imagePreview.loadFromData(r->readAll());
	emit finishedLoadingPreview(this);
}
void Image::parseTags(QNetworkReply* r)
{
	QString source = r->readAll();
	QRegExp rx(m_parent->site().value("Regex/Tags"));
	rx.setMinimal(true);

	int pos = 0;
	while ((pos = rx.indexIn(source, pos)) != -1)
	{
		pos += rx.matchedLength();
		QString type = rx.cap(1), tag = rx.cap(2).replace(" ", "_");
		int count = rx.cap(3).toInt();
		for (int i = 0; i < m_tags.count(); i++)
		{
			if (tag == m_tags.at(i)->text())
			{ m_tags[i] = new Tag(tag, type, count); }
		}
	}

	emit finishedLoadingTags(this);
}

QString Image::filter(QStringList filters)
{
	QStringList types = QStringList() << "rating" << "source" << "id" << "width" << "height" << "score" << "mpixels";
	bool invert;
	QString filter, type;
	for (int i = 0; i < filters.count(); i++)
	{
		invert = false;
		filter = filters.at(i);
		if (filter.startsWith('-'))
		{
			filter = filter.right(filter.length()-1);
			invert = true;
		}
		if (filter.contains(":"))
		{
			type = filter.section(':', 0, 0).toLower();
			filter = filter.section(':', 1).toLower();
			if (!types.contains(type))
			{ return QObject::tr("unknown type \"%1\" (available types: \"%2\")").arg(filter, types.join("\", \"")); }
			if (type == "rating")
			{
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
			else if (type == "id" || type == "width" || type == "height" || type == "score" || type == "mpixels")
			{
				int input = 0;
				if (type == "id")		{ input = m_id;								}
				if (type == "width")	{ input = m_size.width();					}
				if (type == "height")	{ input = m_size.height();					}
				if (type == "score")	{ input = m_score;							}
				if (type == "mpixels")	{ input = m_size.width()*m_size.height();	}

				bool cond;
				if (filter.startsWith("..") || filter.startsWith("<="))
				{ cond = input <= filter.right(filter.size()-2).toInt(); }
				else if (filter.endsWith  ("..") || filter.startsWith(">="))
				{ cond = input >= filter.right(filter.size()-2).toInt(); }
				else if (filter.startsWith("<"))
				{ cond = input < filter.right(filter.size()-1).toInt(); }
				else if (filter.startsWith(">"))
				{ cond = input > filter.right(filter.size()-1).toInt(); }
				else if (filter.contains(".."))
				{ cond = input >= filter.left(filter.indexOf("..")).toInt() && input <= filter.right(filter.size()-filter.indexOf("..")-2).toInt();	}
				else
				{ cond = input == filter.toInt(); }

				if (!cond && !invert)
				{ return QObject::tr("image's %1 does not match").arg(type); }
				if (cond && invert)
				{ return QObject::tr("image's %1 match").arg(type); }
			}
		}
		else if (!filter.isEmpty())
		{
			bool cond = false;
			for (int t = 0; t < m_tags.count(); t++)
			{
				if (m_tags.at(t)->text().toLower() == filter.toLower())
				{ cond = true; break; }
			}
			if (!cond && !invert)
			{ return QObject::tr("image does not contains \"%1\"").arg(filter); }
			if (cond && invert)
			{ return QObject::tr("image contains \"%1\"").arg(filter); }
		}
	}
	return QString();
}

QString		Image::url()			{ return m_url;				}
QString		Image::md5()			{ return m_md5;				}
QString		Image::author()			{ return m_author;			}
QString		Image::status()			{ return m_status;			}
QString		Image::rating()			{ return m_rating;			}
QString		Image::source()			{ return m_source;			}
QString		Image::site()			{ return m_site;			}
QList<Tag*>	Image::tags()			{ return m_tags;			}
int			Image::id()				{ return m_id;				}
int			Image::score()			{ return m_score;			}
int			Image::parentId()		{ return m_parentId;		}
int			Image::fileSize()		{ return m_fileSize;		}
int			Image::width()			{ return m_size.width();	}
int			Image::height()			{ return m_size.height();	}
int			Image::authorId()		{ return m_authorId;		}
QDateTime	Image::createdAt()		{ return m_createdAt;		}
bool		Image::hasChildren()	{ return m_hasChildren;		}
bool		Image::hasNote()		{ return m_hasNote;			}
bool		Image::hasComments()	{ return m_hasComments;		}
QUrl		Image::fileUrl()		{ return m_fileUrl;			}
QUrl		Image::sampleUrl()		{ return m_sampleUrl;		}
QUrl		Image::previewUrl()		{ return m_previewUrl;		}
QUrl		Image::pageUrl()		{ return m_pageUrl;			}
QSize		Image::size()			{ return m_size;			}
QPixmap		Image::previewImage()	{ return m_imagePreview;	}
Page		*Image::page()			{ return m_parent;			}
