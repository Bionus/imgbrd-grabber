#include "image.h"
#include "functions.h"



Image::Image(QMap<QString, QString> details, int timezonedecay, Page* parent)
{
	m_url = details.contains("file_url") ? details["file_url"] : "";
	m_site = details.contains("site") ? details["site"] : m_url.section("/", 2, 2);
	m_md5 = details.contains("md5") ? details["md5"] : "";
	m_author = details.contains("author") ? details["author"] : "";
	m_rating = details.contains("rating") ? details["rating"] : "";
	QStringMap assoc;
		assoc["s"] = tr("Safe");
		assoc["q"] = tr("Questionable");
		assoc["e"] = tr("Explicit");
	if (assoc.contains(m_rating))
	{ m_rating = assoc[m_rating]; }
	m_source = details.contains("source") ? details["source"] : "";
	m_tags = details.contains("tags") ? details["tags"].split(" ") : QStringList();
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
}
Image::~Image()
{ }

void Image::loadPreview()
{
	// Load image, and when finished parse result
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parse(QNetworkReply*)));
	manager->get(QNetworkRequest(m_previewUrl));
}

void Image::parse(QNetworkReply* r)
{
	m_imagePreview.loadFromData(r->readAll());
	emit finishedLoadingPreview(this);
}

QString Image::filter(QStringList filters)
{
	QStringList types = QStringList() << "rating";
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
				if (!m_rating.toLower().startsWith(filter.left(1)) && !invert)
				{ return QObject::tr("image is not \"%1\"").arg(filter); }
				if (m_rating.toLower().startsWith(filter.left(1)) && invert)
				{ return QObject::tr("image is \"%1\"").arg(filter); }
			}
		}
		else if (!filter.isEmpty())
		{
			if (!m_tags.contains(filter) && !invert)
			{ return QObject::tr("image does not contains \"%1\"").arg(filter); }
			if (m_tags.contains(filter) && invert)
			{ return QObject::tr("image contains \"%1\"").arg(filter); }
		}
	}
	return QString();
}

QString		Image::url()			{ return m_url;				}
QString		Image::md5()			{ return m_md5;				}
QString		Image::author()			{ return m_author;			}
QString		Image::rating()			{ return m_rating;			}
QString		Image::source()			{ return m_source;			}
QString		Image::site()			{ return m_site;			}
QStringList	Image::tags()			{ return m_tags;			}
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
