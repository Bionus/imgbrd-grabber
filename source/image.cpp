#include "image.h"
#include "functions.h"



Image::Image(QMap<QString, QString> details, int timezonedecay)
{
	m_url = details.contains("file_url") ? details["file_url"] : "";
	m_md5 = details.contains("md5") ? details["md5"] : "";
	m_author = details.contains("author") ? details["author"] : "";
	m_rating = details.contains("rating") ? details["rating"] : "";
	m_source = details.contains("source") ? details["source"] : "";
	m_tags = details.contains("tags") ? details["tags"].split(" ") : QStringList();
	m_id = details.contains("id") ? details["id"].toInt() : 0;
	m_score = details.contains("score") ? details["score"].toInt() : 0;
	m_parent_id = details.contains("parent_id") ? details["parent_id"].toInt() : 0;
	m_file_size = details.contains("file_size") ? details["file_size"].toInt() : 0;
	m_created_at = QDateTime();
	if (details.contains("created_at"))
	{
		if (details["created_at"].toInt() != 0)
		{ m_created_at.setTime_t(details["created_at"].toInt()); }
		else
		{ m_created_at = qDateTimeFromString(details["created_at"], timezonedecay); }
	}
	m_has_children = details.contains("has_children") ? details["has_children"] == "true" : false;
	m_has_notes = details.contains("has_notes") ? details["has_notes"] == "true" : false;
	m_has_comments = details.contains("has_comments") ? details["has_comments"] == "true" : false;
	m_page_url = details.contains("page_url") ? QUrl(details["page_url"]) : QUrl();
	m_file_url = details.contains("file_url") ? QUrl(details["file_url"]) : QUrl();
	m_sample_url = details.contains("sample_url") ? QUrl(details["sample_url"]) : QUrl();
	m_preview_url = details.contains("preview_url") ? QUrl(details["preview_url"]) : QUrl();
	m_size = QSize(details.contains("width") ? details["width"].toInt() : 0, details.contains("height") ? details["height"].toInt() : 0);
}

QString		Image::url()			{ return m_url;				}
QString		Image::md5()			{ return m_md5;				}
QString		Image::author()			{ return m_author;			}
QString		Image::rating()			{ return m_rating;			}
QString		Image::source()			{ return m_source;			}
QStringList	Image::tags()			{ return m_tags;			}
int			Image::id()				{ return m_id;				}
int			Image::score()			{ return m_score;			}
int			Image::parent_id()		{ return m_parent_id;		}
int			Image::file_size()		{ return m_file_size;		}
QDateTime	Image::created_at()		{ return m_created_at;		}
bool		Image::has_children()	{ return m_has_children;	}
bool		Image::has_notes()		{ return m_has_notes;		}
bool		Image::has_comments()	{ return m_has_comments;	}
QUrl		Image::page_url()		{ return m_page_url;		}
QUrl		Image::file_url()		{ return m_file_url;		}
QUrl		Image::sample_url()		{ return m_sample_url;		}
QUrl		Image::preview_url()	{ return m_preview_url;		}
QSize		Image::size()			{ return m_size;			}
