#include "tag.h"
#include <QRegularExpression>
#include <QSet>
#include <QTextDocument>
#include <QtMath>
#include "functions.h"


QMap<int, QString> stringListToMap(const QStringList &list)
{
	QMap<int, QString> ret;
	for (int i = 0; i < list.count(); ++i)
		ret.insert(i, list[i]);
	return ret;
}

Tag::Tag()
	: m_type(TagType("unknown")), m_count(0)
{ }

Tag::Tag(const QString &text, const QString &type, int count, const QStringList &related)
	: Tag(text, TagType(type), count, related)
{ }

Tag::Tag(const QString &text, const TagType &type, int count, const QStringList &related)
	: Tag(0, text, type, count, related)
{ }

Tag::Tag(int id, const QString &text, const TagType &type, int count, const QStringList &related)
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
		static QStringList prep = QStringList() << "artist" << "copyright" << "character" << "model" << "species" << "unknown" << "oc";
		QString pre = Tag::GetType(m_text.left(sepPos));
		int prepIndex = prep.indexOf(pre);
		if (prepIndex != -1)
		{
			m_type = TagType(Tag::GetType(prep[prepIndex], stringListToMap(prep)));
			m_text = m_text.mid(sepPos + 1);
		}
	}
}

Tag Tag::FromCapture(const QRegularExpressionMatch &match, const QStringList &groups)
{
	QMap<QString, QString> data = multiMatchToMap(match, groups);

	// Tag
	QString tag;
	if (data.contains("tag"))
	{
		tag = data["tag"].replace(" ", "_").replace("&amp;", "&").trimmed();
	}

	// Type
	QString type;
	if (data.contains("type"))
	{
		static QStringList types = QStringList() << "general" << "artist" << "unknown" << "copyright" << "character" << "species";
		type = Tag::GetType(data.value("type").trimmed(), stringListToMap(types));
	}
	if (type.isEmpty())
	{ type = "unknown"; }

	// Count
	int count = 0;
	if (data.contains("count"))
	{
		QString countStr = data.value("count").toLower().trimmed();
		countStr.remove(',');
		if (countStr.endsWith('k'))
		{
			QStringRef withoutK = countStr.leftRef(countStr.length() - 1).trimmed();
			count = qRound(withoutK.toFloat() * 1000);
		}
		else
		{ count = countStr.toInt(); }
	}

	return Tag(tag, type, count);
}

QList<Tag> Tag::FromRegexp(const QString &rx, const QString &source)
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

QString Tag::GetType(QString type, QMap<int, QString> ids)
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
		if (ids.contains(typeId))
			return ids[typeId];
	}

	return type;
}

void Tag::setId(int id)						{ m_id = id;		}
void Tag::setText(const QString &text)		{ m_text = text;	}
void Tag::setType(const TagType &type)		{ m_type = type;	}
void Tag::setCount(int count)				{ m_count = count;	}
void Tag::setRelated(const QStringList &r)	{ m_related = r;	}

int			Tag::id() const			{ return m_id;		}
QString		Tag::text() const		{ return m_text;	}
TagType		Tag::type() const		{ return m_type;	}
int			Tag::count() const		{ return m_count;	}
QStringList	Tag::related() const	{ return m_related;	}

bool sortTagsByType(const Tag &s1, const Tag &s2)
{
	static QStringList typeOrder = QStringList() << "unknown" << "model" << "species" << "artist" << "character" << "copyright";
	int t1 = typeOrder.indexOf(s1.type().name());
	int t2 = typeOrder.indexOf(s2.type().name());
	return t1 == t2 ? sortTagsByName(s1, s2) : t1 > t2;
}
bool sortTagsByName(const Tag &s1, const Tag &s2)
{ return s1.text().localeAwareCompare(s2.text()) < 0; }
bool sortTagsByCount(const Tag &s1, const Tag &s2)
{ return s1.count() > s2.count(); }

bool operator==(const Tag &t1, const Tag &t2)
{
	return QString::compare(t1.text(), t2.text(), Qt::CaseInsensitive) == 0
		&& (t1.type() == t2.type() || t1.type().name() == "unknown" || t2.type().name() == "unknown");
}
