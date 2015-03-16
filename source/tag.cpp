#include <QStringList>
#include "tag.h"
#include <QDebug>



Tag::Tag(QString text, QString type, int count, QStringList related)
	: m_text(text), m_type(type), m_count(count), m_related(related)
{
	m_text.replace("&#39;", "'");

	if (type.contains(' '))
	{ m_type = type.left(type.indexOf(' ')); }

	if (text.endsWith("(artist)") && type == "unknown")
	{ m_type = "artist"; }

	QRegExp reg("(artist|copyright|character|model):(.+)");
	if (reg.exactMatch(text) && type == "unknown")
	{
		m_type = reg.cap(1);
		m_text = reg.cap(2);
	}
}
Tag::~Tag()
{ }

/**
 * Return the colored tag.
 * @param favs The list of the user's favorite tags.
 * @return The HTML colored tag.
 */
QString Tag::stylished(QStringList favs) const
{
	if (favs.contains(m_text))
        return "<span style=\"color:pink\">"+m_text+"</span>";

	return m_text;
}

void Tag::setText(QString text)		{ m_text = text;	}
void Tag::setType(QString type)		{ m_type = type;	}
void Tag::setCount(int count)		{ m_count = count;	}
void Tag::setRelated(QStringList r)	{ m_related = r;	}

QString		Tag::text() const		{ return m_text;	}
QString		Tag::type() const		{ return m_type;	}
int			Tag::shortType() const	{ return m_type == "general" ? 0 : (m_type == "artist" ? 1 : (m_type == "copyright" ? 3 : 4)); }
int			Tag::count() const		{ return m_count;	}
QStringList	Tag::related() const	{ return m_related;	}

QString Tag::typedText() const
{
	return (m_type != "general" ? m_type + ":" : "") + m_text;
}

bool sortByFrequency(Tag s1, Tag s2)
{ return s1.count() > s2.count(); }

bool operator==(const Tag &t1, const Tag &t2)
{
	return t1.text() == t2.text() && t1.type() == t2.type();
}
