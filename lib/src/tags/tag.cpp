#include "tag.h"
#include <QTextDocument>
#include <QSettings>
#include <QSet>
#include "models/favorite.h"
#include "models/profile.h"


Tag::Tag()
	: m_type(TagType("unknown")), m_count(0)
{ }

Tag::Tag(QString text, QString type, int count, QStringList related)
	: Tag(text, TagType(type), count, related)
{ }

Tag::Tag(QString text, TagType type, int count, QStringList related)
	: Tag(0, text, type, count, related)
{ }

Tag::Tag(int id, QString text, TagType type, int count, QStringList related)
	: m_id(id), m_type(TagType(type)), m_count(count), m_related(related)
{
	static QStringList weakTypes = QStringList() << "unknown" << "origin";

	// Decode HTML entities in the tag text
	QTextDocument htmlEncoded;
	htmlEncoded.setHtml(text);
	m_text = htmlEncoded.toPlainText().replace(' ', '_');

	// Sometimes a type is found with multiple words, only the first is relevant
	int typeSpace = m_type.name().indexOf(' ');
	if (typeSpace != -1)
	{ m_type = TagType(m_type.name().left(typeSpace)); }

	// Some artist names end with " (artist)" so we can guess their type
	if (m_text.endsWith("(artist)") && weakTypes.contains(m_type.name()))
	{
		m_type = TagType("artist");
		m_text = m_text.left(m_text.length() - 9);
	}

	if (m_text.contains(':') && weakTypes.contains(m_type.name()))
	{
		QStringList prep = QStringList() << "artist" << "copyright" << "character" << "model" << "species" << "unknown" << "oc";
		foreach (QString pre, prep)
		{
			if (m_text.startsWith(pre + ":"))
			{
				m_type = TagType(Tag::GetType(pre, prep));
				m_text = m_text.mid(pre.length() + 1);
			}
		}
	}
}
Tag::~Tag()
{ }

Tag Tag::FromCapture(QStringList caps, QStringList order)
{
	QString type;
	QString tag;
	int count = 0;

	// Most common tag orders
	if (order.empty())
	{
		switch (caps.count())
		{
			case 4:	order << "type" << "" << "count" << "tag";	break;
			case 3:	order << "type" << "tag" << "count";		break;
			case 2:	order << "type" << "tag";					break;
			case 1:	order << "tag";								break;
		}
	}

	int max = qMin(order.size(), caps.size());
	for (int o = 0; o < max; o++)
	{
		QString ord = order[o];
		QString cap = caps[o];

		if (ord == "tag" && tag.isEmpty())
		{
			tag = cap.replace(" ", "_").replace("&amp;", "&").trimmed();
		}
		else if (ord == "type" && type.isEmpty())
		{
			type = Tag::GetType(cap.trimmed(), QStringList() << "general" << "artist" << "unknown" << "copyright" << "character" << "species");
		}
		else if (ord == "count" && count == 0)
		{
			QString countStr = cap.toLower().trimmed();
			countStr.remove(',');
			count = countStr.endsWith('k', Qt::CaseInsensitive) ? countStr.left(countStr.length() - 1).toFloat() * 1000 : countStr.toInt();
		}
	}

	if (type.isEmpty())
	{ type = "unknown"; }

	return Tag(tag, type, count);
}

QList<Tag> Tag::FromRegexp(QString rx, QStringList order, const QString &source)
{
	QRegExp rxtags(rx);
	rxtags.setMinimal(true);

	QList<Tag> ret;
	QSet<QString> got;

	int pos = 0;
	while ((pos = rxtags.indexIn(source, pos)) != -1)
	{
		pos += rxtags.matchedLength();

		QStringList caps = rxtags.capturedTexts();
		caps.removeFirst();
		Tag tag = Tag::FromCapture(caps, order);

		if (!got.contains(tag.text()))
		{
			got.insert(tag.text());
			ret.append(tag);
		}
	}

	return ret;
}

QString Tag::GetType(QString type, QStringList ids)
{
	type = type.toLower().trimmed();
	if (type.contains(", "))
		type = type.split(", ").at(0).trimmed();

	if (type == "series")
		return "copyright";
	if (type == "mangaka")
		return "artist";
	if (type == "game")
		return "copyright";
	if (type == "studio")
		return "circle";
	if (type == "source")
		return "general";
	if (type == "character group")
		return "general";
	if (type == "oc")
		return "character";

	if (type.length() == 1)
	{
		int typeId = type.toInt();
		if (typeId >= 0 && typeId < ids.count())
			return ids[typeId];
	}

	return type;
}

/**
 * Converts a QFont to a CSS string.
 * @param	font	The font to convert.
 * @return	The CSS font.
 */
QString Tag::qFontToCss(QFont font)
{
	QString style;
	switch (font.style())
	{
		case QFont::StyleNormal:	style = "normal";	break;
		case QFont::StyleItalic:	style = "italic";	break;
		case QFont::StyleOblique:	style = "oblique";	break;
	}

	QString size;
	if (font.pixelSize() == -1)
	{ size = QString::number(font.pointSize())+"pt"; }
	else
	{ size = QString::number(font.pixelSize())+"px"; }

	// Should be "font.weight() * 8 + 100", but linux doesn't handle weight the same way windows do
	QString weight = QString::number(font.weight() * 8);

	QStringList decorations;
	if (font.strikeOut())	{ decorations.append("line-through");	}
	if (font.underline())	{ decorations.append("underline");		}

	return "font-family:'"+font.family()+"'; font-size:"+size+"; font-style:"+style+"; font-weight:"+weight+"; text-decoration:"+(decorations.isEmpty() ? "none" : decorations.join(" "))+";";
}

QStringList Tag::Stylished(QList<Tag> tags, Profile *profile, bool count, bool nounderscores, QString sort)
{
	QStringList ignored = profile->getIgnored();
	QStringList blacklisted = profile->getBlacklist();

	// Sort tag list
	if (sort == "type")
		qSort(tags.begin(), tags.end(), sortTagsByType);
	else if (sort == "name")
		qSort(tags.begin(), tags.end(), sortTagsByName);
	else if (sort == "count")
		qSort(tags.begin(), tags.end(), sortTagsByCount);

	QStringList t;
	for (Tag tag : tags)
		t.append(tag.stylished(profile, ignored, blacklisted, count, nounderscores));

	return t;
}

/**
 * Return the colored tag.
 * @param favs The list of the user's favorite tags.
 * @return The HTML colored tag.
 */
QString Tag::stylished(Profile *profile, QStringList ignored, QStringList blacklisted, bool count, bool nounderscores) const
{
	static const QStringList tlist = QStringList() << "artists" << "circles" << "copyrights" << "characters" << "models" << "generals" << "favorites" << "blacklisteds" << "ignoreds" << "favorites";
	static const QStringList defaults = QStringList() << "#aa0000" << "#55bbff" << "#aa00aa" << "#00aa00" << "#0000ee" << "#000000" << "#ffc0cb" << "#000000" << "#999999" << "#ffcccc";

	// Guess the correct tag family
	QString key = tlist.contains(type().name()+"s") ? type().name() + "s" : "generals";
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
	QString style = "color:" + color + "; " + qFontToCss(font);

	QString ret;
	ret = "<a href=\"" + text() + "\" style=\"" + style + "\">" + (nounderscores ? text().replace('_', ' ') : text()) + "</a>";
	if (count && this->count() > 0)
		ret += " <span style=\"color:#aaa\">(" + QString("%L1").arg(this->count()) + ")</span>";

	return ret;
}

void Tag::setId(int id)				{ m_id = id;		}
void Tag::setText(QString text)		{ m_text = text;	}
void Tag::setType(TagType type)		{ m_type = type;	}
void Tag::setCount(int count)		{ m_count = count;	}
void Tag::setRelated(QStringList r)	{ m_related = r;	}

int			Tag::id() const			{ return m_id;		}
QString		Tag::text() const		{ return m_text;	}
TagType		Tag::type() const		{ return m_type;	}
int			Tag::count() const		{ return m_count;	}
QStringList	Tag::related() const	{ return m_related;	}

QString Tag::typedText() const
{
	return (m_type.name() != "general" ? m_type.name() + ":" : "") + m_text;
}

bool sortTagsByType(Tag s1, Tag s2)
{
	static QStringList typeOrder = QStringList() << "unknown" << "model" << "species" << "artist" << "character" << "copyright";
	int t1 = typeOrder.indexOf(s1.type().name());
	int t2 = typeOrder.indexOf(s2.type().name());
	return t1 == t2 ? sortTagsByName(s1, s2) : t1 > t2;
}
bool sortTagsByName(Tag s1, Tag s2)
{ return s1.text().localeAwareCompare(s2.text()) < 0; }
bool sortTagsByCount(Tag s1, Tag s2)
{ return s1.count() > s2.count(); }

bool operator==(const Tag &t1, const Tag &t2)
{
	return t1.text() == t2.text() && (t1.type() == t2.type() || t1.type().name() == "unknown" || t2.type().name() == "unknown");
}
