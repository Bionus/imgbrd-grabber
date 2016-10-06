#include <QTextDocument>
#include <QSettings>
#include "tag.h"
#include "functions.h"


Tag::Tag()
	: m_type("unknown"), m_count(0)
{ }

Tag::Tag(QString text, QString type, int count, QStringList related)
	: m_type(type), m_count(count), m_related(related)
{
	// Decode HTML entities in the tag text
	QTextDocument htmlEncoded;
	htmlEncoded.setHtml(text);
	m_text = htmlEncoded.toPlainText().replace(' ', '_');

	// Sometimes a type is found with multiple words, only the first is relevant
	int typeSpace = type.indexOf(' ');
	if (typeSpace != -1)
	{ m_type = type.left(typeSpace); }

	// Some artist names end with " (artist)" so we can guess their type
	if (m_text.endsWith("(artist)") && type == "unknown")
	{
		m_type = "artist";
		m_text = m_text.left(m_text.length() - 9);
	}

	if (m_type == "unknown" && m_text.contains(':'))
	{
		QStringList prep = QStringList() << "artist" << "copyright" << "character" << "model" << "unknown";
		foreach (QString pre, prep)
		{
			if (m_text.startsWith(pre + ":"))
			{
				m_type = pre;
				m_text = m_text.mid(pre.length() + 1);
			}
		}
	}
}
Tag::~Tag()
{ }
#include <QDebug>
/**
 * Return the colored tag.
 * @param favs The list of the user's favorite tags.
 * @return The HTML colored tag.
 */
QString Tag::stylished(Profile *profile, QStringList ignored, QStringList blacklisted, bool count) const
{
	QStringList tlist = QStringList() << "artists" << "circles" << "copyrights" << "characters" << "models" << "generals" << "favorites" << "blacklisteds" << "ignoreds" << "favorites";
	QStringList defaults = QStringList() << "#aa0000" << "#55bbff" << "#aa00aa" << "#00aa00" << "#0000ee" << "#000000" << "#ffc0cb" << "#000000" << "#999999" << "#ffcccc";

	// Guess the correct tag family
	QString key = tlist.contains(type()+"s") ? type() + "s" : "generals";
	if (blacklisted.contains(text(), Qt::CaseInsensitive))
		key = "blacklisteds";
	if (ignored.contains(text(), Qt::CaseInsensitive))
		key = "ignoreds";
	for (Favorite fav : profile->getFavorites())
		if (fav.getName() == m_text)
			key = "favorites";

	QFont font;
	font.fromString(profile->getSettings()->value("Coloring/Fonts/" + key).toString());
	QString color = profile->getSettings()->value("Coloring/Colors/" + key, defaults.at(tlist.indexOf(key))).toString();
	QString style = "color:"+color+"; "+qfonttocss(font);

	QString ret;
	ret = "<a href=\"" + text() + "\" style=\"" + style + "\">" + text() + "</a>";
	if (count && this->count() > 0)
		ret += " <span style=\"color:#aaa\">(" + QString("%L1").arg(this->count()) + ")</span>";

	return ret;
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
	return t1.text() == t2.text() && (t1.type() == t2.type() || t1.type() == "unknown" || t2.type() == "unknown");
}
