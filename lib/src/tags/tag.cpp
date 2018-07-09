#include "tag.h"
#include <QRegularExpression>
#include <QSet>
#include "functions.h"
#include "tag-type.h"


QMap<int, QString> stringListToMap(const QStringList &list)
{
	QMap<int, QString> ret;
	for (int i = 0; i < list.count(); ++i)
		ret.insert(i, list[i]);
	return ret;
}

Tag::Tag()
	: m_id(0), m_type(TagType()), m_count(0)
{ }

Tag::Tag(const QString &text, const QString &type, int count, const QStringList &related)
	: Tag(text, TagType(type), count, related)
{ }

Tag::Tag(const QString &text, const TagType &type, int count, const QStringList &related)
	: Tag(0, text, type, count, related)
{ }

Tag::Tag(int id, const QString &text, TagType type, int count, QStringList related)
	: m_id(id), m_type(std::move(type)), m_count(count), m_related(std::move(related))
{
	static QStringList weakTypes = QStringList() << QStringLiteral("unknown") << QStringLiteral("origin");

	// Decode HTML entities in the tag text
	m_text = decodeHtmlEntities(text).replace(' ', '_');

	// Sometimes a type is found with multiple words, only the first is relevant
	const int typeSpace = m_type.name().indexOf(' ');
	if (typeSpace != -1)
	{ m_type = TagType(m_type.name().left(typeSpace)); }

	// Some artist names end with " (artist)" so we can guess their type
	if (m_text.endsWith(QLatin1String("(artist)")) && weakTypes.contains(m_type.name()))
	{
		m_type = TagType(QStringLiteral("artist"));
		m_text = m_text.left(m_text.length() - 9);
	}

	const int sepPos = m_text.indexOf(':');
	if (sepPos != -1 && weakTypes.contains(m_type.name()))
	{
		static QStringList prep = QStringList()
			<< QStringLiteral("artist")
			<< QStringLiteral("copyright")
			<< QStringLiteral("character")
			<< QStringLiteral("model")
			<< QStringLiteral("species")
			<< QStringLiteral("meta")
			<< QStringLiteral("unknown")
			<< QStringLiteral("oc");

		const QString pre = Tag::GetType(m_text.left(sepPos));
		const int prepIndex = prep.indexOf(pre);
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
		static QStringList types = QStringList() << "general" << "artist" << "unknown" << "copyright" << "character" << "species" << "meta";
		type = Tag::GetType(data.value("type").trimmed(), stringListToMap(types));
	}

	// Count
	int count = 0;
	if (data.contains("count"))
	{
		QString countStr = data.value("count").toLower().trimmed();
		countStr.remove(',');
		if (countStr.endsWith('k'))
		{
			QStringRef withoutK = countStr.leftRef(countStr.length() - 1).trimmed();
			count = qRound(withoutK.toDouble() * 1000);
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
		const auto &match = matches.next();
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

	if (type == QLatin1String("series"))
		return QStringLiteral("copyright");
	if (type == QLatin1String("mangaka"))
		return QStringLiteral("artist");
	if (type == QLatin1String("game"))
		return QStringLiteral("copyright");
	if (type == QLatin1String("studio"))
		return QStringLiteral("circle");
	if (type == QLatin1String("source"))
		return QStringLiteral("general");
	if (type == QLatin1String("character group"))
		return QStringLiteral("general");
	if (type == QLatin1String("oc"))
		return QStringLiteral("character");

	if (type.length() == 1)
	{
		const int typeId = type.toInt();
		if (ids.contains(typeId))
			return ids[typeId];
	}

	return type;
}

void Tag::setId(int id)						{ m_id = id;		}
void Tag::setText(const QString &text)		{ m_text = text;	}
void Tag::setType(const TagType &type)		{ m_type = type;	}
void Tag::setCount(int count)				{ m_count = count;	}
void Tag::setRelated(const QStringList &related)	{ m_related = related;	}

int Tag::id() const						{ return m_id;		}
const QString &Tag::text() const		{ return m_text;	}
const TagType &Tag::type() const		{ return m_type;	}
int Tag::count() const					{ return m_count;	}
const QStringList &Tag::related() const	{ return m_related;	}

bool sortTagsByType(const Tag &s1, const Tag &s2)
{
	static QStringList typeOrder = QStringList()
		<< QStringLiteral("unknown")
		<< QStringLiteral("model")
		<< QStringLiteral("meta")
		<< QStringLiteral("species")
		<< QStringLiteral("artist")
		<< QStringLiteral("character")
		<< QStringLiteral("copyright");

	const int t1 = typeOrder.indexOf(s1.type().name());
	const int t2 = typeOrder.indexOf(s2.type().name());
	return t1 == t2 ? sortTagsByName(s1, s2) : t1 > t2;
}
bool sortTagsByName(const Tag &s1, const Tag &s2)
{ return s1.text().localeAwareCompare(s2.text()) < 0; }
bool sortTagsByCount(const Tag &s1, const Tag &s2)
{ return s1.count() > s2.count(); }

bool operator==(const Tag &t1, const Tag &t2)
{
	return QString::compare(t1.text(), t2.text(), Qt::CaseInsensitive) == 0
		&& (t1.type() == t2.type() || t1.type().isUnknown() || t2.type().isUnknown());
}
