#include "tags/tag-database-sqlite.h"
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlField>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QVariant>
#include <utility>
#include "logger.h"
#include "tags/tag.h"


TagDatabaseSqlite::TagDatabaseSqlite(const QString &typeFile, QString tagFile)
	: TagDatabase(typeFile), m_tagFile(std::move(tagFile)), m_count(-1)
{}

bool TagDatabaseSqlite::open()
{
	// Don't re-open databases
	if (m_database.isOpen()) {
		return true;
	}

	// Load and connect to the database
	m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), "Tag database - " + m_tagFile);
	m_database.setDatabaseName(m_tagFile);
	if (!m_database.open()) {
		log(QStringLiteral("Could not open tag database '%1': %2").arg(m_tagFile, m_database.lastError().text()), Logger::Error);
		return false;
	}

	// Create schema if necessary
	QSqlQuery createQuery(m_database);
	createQuery.prepare(QStringLiteral("CREATE TABLE IF NOT EXISTS tags (id INT, tag VARCHAR(255), ttype INT);"));
	if (!createQuery.exec()) {
		log(QStringLiteral("Could not create tag database schema: %1").arg(createQuery.lastError().text()), Logger::Error);
		return false;
	}

	return TagDatabase::open();
}

bool TagDatabaseSqlite::close()
{
	m_database.close();

	return TagDatabase::close();
}

bool TagDatabaseSqlite::load()
{
	return TagDatabase::load();
}

bool TagDatabaseSqlite::save()
{
	return TagDatabase::save();
}

void TagDatabaseSqlite::setTags(const QList<Tag> &tags, bool createTagTypes)
{
	QSqlQuery clearQuery(m_database);
	clearQuery.prepare(QStringLiteral("DELETE FROM tags"));
	if (!clearQuery.exec()) {
		log(QStringLiteral("SQL error when clearing tags: %1").arg(clearQuery.lastError().text()), Logger::Error);
		return;
	}

	if (!m_database.transaction()) {
		return;
	}

	QSqlQuery addQuery(m_database);
	addQuery.prepare(QStringLiteral("INSERT INTO tags (id, tag, ttype) VALUES (:id, :tag, :ttype)"));

	for (const Tag &tag : tags) {
		addQuery.bindValue(":id", tag.id());
		addQuery.bindValue(":tag", tag.text());
		addQuery.bindValue(":ttype", m_tagTypeDatabase.get(tag.type(), createTagTypes));
		if (!addQuery.exec()) {
			log(QStringLiteral("SQL error when adding tag: %1").arg(addQuery.lastError().text()), Logger::Error);
			return;
		}
	}

	if (!m_database.commit()) {
		return;
	}

	m_count = -1;
}

QMap<QString, TagType> TagDatabaseSqlite::getTagTypes(const QStringList &tags) const
{
	QMap<QString, TagType> ret;

	// Escape values
	QStringList formatted;
	QSqlDriver *driver = m_database.driver();
	for (const QString &tag : tags) {
		if (m_cache.contains(tag)) {
			ret.insert(tag, m_cache[tag]);
		} else {
			QSqlField f;
			f.setType(QVariant::String);
			f.setValue(tag);
			formatted.append(driver->formatValue(f));
		}
	}

	// If all values have already been loaded from the memory cache
	if (formatted.isEmpty()) {
		return ret;
	}

	// Execute query
	const QString sql = "SELECT tag, ttype FROM tags WHERE tag IN (" + formatted.join(",") + ")";
	QSqlQuery query(m_database);
	query.setForwardOnly(true);
	if (!query.exec(sql)) {
		log(QStringLiteral("SQL error when getting tags: %1").arg(query.lastError().text()), Logger::Error);
		return ret;
	}

	const int idTag = query.record().indexOf("tag");
	const int idTtype = query.record().indexOf("ttype");
	while (query.next()) {
		const QString tag = query.value(idTag).toString();
		const int typeId = query.value(idTtype).toInt();
		if (!m_tagTypeDatabase.contains(typeId)) {
			continue;
		}
		const TagType type = m_tagTypeDatabase.get(typeId);
		ret.insert(tag, type);
		m_cache.insert(tag, type);
	}

	return ret;
}

int TagDatabaseSqlite::count() const
{
	if (m_count != -1) {
		return m_count;
	}

	QSqlQuery query(m_database);
	const QString sql = QStringLiteral("SELECT COUNT(*) FROM tags");
	if (!query.exec(sql) || !query.next()) {
		log(QStringLiteral("SQL error when getting tag count: %1").arg(query.lastError().text()), Logger::Error);
		return -1;
	}

	m_count = query.value(0).toInt();
	return m_count;
}
