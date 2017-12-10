#include "tag.h"
#include <QRegularExpression>
#include <QSet>
#include <QTextDocument>
#include <QtMath>


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
	: m_id(id), m_type(type), m_count(count), m_related(related)
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

	int sepPos = m_text.indexOf(':');
	if (sepPos != -1 && weakTypes.contains(m_type.name()))
	{
		QStringList prep = QStringList() << "artist" << "copyright" << "character" << "model" << "species" << "unknown" << "oc";
		QString pre = Tag::GetType(m_text.left(sepPos), QStringList());
		int prepIndex = prep.indexOf(pre);
		if (prepIndex != -1)
		{
			m_type = TagType(Tag::GetType(prep[prepIndex], prep));
			m_text = m_text.mid(sepPos + 1);
		}
	}
}

Tag Tag::FromCapture(const QRegularExpressionMatch &match, const QStringList &groups)
{
	// Tag
	QString tag;
	if (groups.contains("tag"))
	{
		tag = match.captured("tag").replace(" ", "_").replace("&amp;", "&").trimmed();
	}

	// Type
	QString type;
	if (groups.contains("type"))
	{
		type = Tag::GetType(match.captured("type").trimmed(), QStringList() << "general" << "artist" << "unknown" << "copyright" << "character" << "species");
	}
	if (type.isEmpty())
	{ type = "unknown"; }

	// Count
	int count = 0;
	if (groups.contains("count"))
	{
		QString countStr = match.captured("count").toLower().trimmed();
		countStr.remove(',');
		count = countStr.endsWith('k', Qt::CaseInsensitive) ? qFloor(countStr.leftRef(countStr.length() - 1).toFloat() * 1000) : countStr.toInt();
	}

	return Tag(tag, type, count);
}

QList<Tag> Tag::FromRegexp(QString rx, const QString &source)
{
	QRegularExpression rxtags(rx);

	QList<Tag> ret;
	QSet<QString> got;

	auto matches = rxtags.globalMatch(source);
	while (matches.hasNext())
	{
		auto match = matches.next();
		Tag tag = Tag::FromCapture(match, rxtags.namedCaptureGroups());

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

void Tag::setId(int id)					{ m_id = id;		}
void Tag::setText(const QString &text)		{ m_text = text;	}
void Tag::setType(const TagType &type)		{ m_type = type;	}
void Tag::setCount(int count)				{ m_count = count;	}
void Tag::setRelated(const QStringList &r)	{ m_related = r;	}

int			Tag::id() const			{ return m_id;		}
QString		Tag::text() const		{ return m_text;	}
TagType		Tag::type() const		{ return m_type;	}
int			Tag::count() const		{ return m_count;	}
QStringList	Tag::related() const	{ return m_related;	}

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
