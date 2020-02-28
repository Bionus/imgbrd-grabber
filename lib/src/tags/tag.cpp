#include "tag.h"
#include <QJsonArray>
#include <utility>
#include "functions.h"
#include "tag-type.h"


Tag::Tag()
	: m_id(0), m_type(TagType()), m_count(0)
{}

Tag::Tag(const QString &text, const QString &type, int count, const QStringList &related)
	: Tag(text, TagType(type), count, related)
{}

Tag::Tag(const QString &text, const TagType &type, int count, const QStringList &related)
	: Tag(0, text, type, count, related)
{}

Tag::Tag(int id, const QString &text, TagType type, int count, QStringList related)
	: m_id(id), m_type(std::move(type)), m_count(count), m_related(std::move(related))
{
	static QStringList weakTypes = QStringList() << QStringLiteral("origin");

	// Decode HTML entities in the tag text
	m_text = decodeHtmlEntities(text).replace(' ', '_');

	if (m_type.isUnknown() || weakTypes.contains(m_type.name())) {
		// Some artist names end with " (artist)" so we can guess their type
		if (m_text.endsWith(QLatin1String("(artist)"))) {
			m_type = TagType(QStringLiteral("artist"));
			m_text = m_text.left(m_text.length() - 9);
		}

		const int sepPos = m_text.indexOf(':');
		if (sepPos != -1) {
			static QMap<int, QString> prep =
			{
				{ 0, QStringLiteral("artist") },
				{ 1, QStringLiteral("copyright") },
				{ 2, QStringLiteral("character") },
				{ 3, QStringLiteral("model") },
				{ 4, QStringLiteral("species") },
				{ 5, QStringLiteral("meta") },
				{ 6, QStringLiteral("unknown") },
				{ 7, QStringLiteral("oc") }
			};

			const QString pre = Tag::GetType(m_text.left(sepPos));
			const int prepIndex = prep.key(pre, -1);
			if (prepIndex != -1) {
				m_type = TagType(Tag::GetType(prep[prepIndex], prep));
				m_text = m_text.mid(sepPos + 1);
			}
		}
	}
}

QString Tag::GetType(QString type, QMap<int, QString> ids)
{
	type = type.toLower().trimmed();
	if (type.contains(", ")) {
		type = type.split(", ").at(0).trimmed();
	}

	if (type == QLatin1String("idol")) {
		return QStringLiteral("model");
	}
	if (type == QLatin1String("series")) {
		return QStringLiteral("copyright");
	}
	if (type == QLatin1String("mangaka")) {
		return QStringLiteral("artist");
	}
	if (type == QLatin1String("game")) {
		return QStringLiteral("copyright");
	}
	if (type == QLatin1String("studio")) {
		return QStringLiteral("circle");
	}
	if (type == QLatin1String("source")) {
		return QStringLiteral("general");
	}
	if (type == QLatin1String("character group")) {
		return QStringLiteral("general");
	}
	if (type == QLatin1String("oc")) {
		return QStringLiteral("character");
	}

	if (type.length() == 1) {
		const int typeId = type.toInt();
		if (ids.contains(typeId)) {
			return ids[typeId];
		}
	}

	return type;
}


void Tag::write(QJsonObject &json) const
{
	json["text"] = m_text;

	if (m_id > 0) {
		json["id"] = m_id;
	}
	if (!m_type.isUnknown()) {
		json["type"] = m_type.name();
	}
	if (m_count >= 0) {
		json["count"] = m_count;
	}
	if (!m_related.isEmpty()) {
		json["related"] = QJsonArray::fromStringList(m_related);
	}
}

bool Tag::read(const QJsonObject &json)
{
	m_text = json["text"].toString();

	if (json.contains("id")) {
		m_id = json["id"].toInt();
	}
	if (json.contains("type")) {
		m_type = TagType(json["type"].toString());
	}
	if (json.contains("count")) {
		m_count = json["count"].toInt();
	}

	// Related
	if (json.contains("related")) {
		QJsonArray related = json["related"].toArray();
		m_related.reserve(related.count());
		for (auto tag : related) {
			m_related.append(tag.toString());
		}
	}

	return true;
}


void Tag::setId(int id) { m_id = id; }
void Tag::setText(const QString &text) { m_text = text; }
void Tag::setType(const TagType &type) { m_type = type; }
void Tag::setCount(int count) { m_count = count; }
void Tag::setRelated(const QStringList &related) { m_related = related; }

int Tag::id() const { return m_id; }
const QString &Tag::text() const { return m_text; }
const TagType &Tag::type() const { return m_type; }
int Tag::count() const { return m_count; }
const QStringList &Tag::related() const { return m_related; }

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
bool operator!=(const Tag &t1, const Tag &t2)
{ return !(t1 == t2); }
