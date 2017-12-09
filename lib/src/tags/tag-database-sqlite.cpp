#include "tags/tag-database-sqlite.h"
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlField>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QVariant>
#include "logger.h"


TagDatabaseSqlite::TagDatabaseSqlite(QString typeFile, QString tagFile)
	: TagDatabase(typeFile), m_tagFile(tagFile), m_count(-1)
{}

bool TagDatabaseSqlite::load()
{
	// Don't reload databases
	if (m_database.isOpen())
		return true;

	// Load tag types
	if (!TagDatabase::load())
		return false;

	// Load and connect to the database
	m_database = QSqlDatabase::addDatabase("QSQLITE");
	m_database.setDatabaseName(m_tagFile);
	if (!m_database.open())
	{
		log(QString("Could not open tag database: %1").arg(m_database.lastError().text()), Logger::Error);
		return false;
	}

	// Create schema if necessary
	QSqlQuery createQuery(m_database);
	createQuery.prepare("CREATE TABLE IF NOT EXISTS tags (id INT, tag VARCHAR(255), ttype INT);");
	if (!createQuery.exec())
	{
		log(QString("Could not create tag database schema: %1").arg(createQuery.lastError().text()), Logger::Error);
		return false;
	}

	return true;
}

bool TagDatabaseSqlite::save()
{
	return true;
}

void TagDatabaseSqlite::setTags(const QList<Tag> &tags)
{
	// Inverted tag type map to get the tag type ID from its name
	QMap<QString, int> tagTypes;
	for (int typeId : m_tagTypes.keys())
		tagTypes.insert(m_tagTypes[typeId].name(), typeId);

	QSqlQuery clearQuery(m_database);
	clearQuery.prepare("DELETE FROM tags");
	if (!clearQuery.exec())
	{
		log(QString("SQL error when clearing tags: %1").arg(clearQuery.lastError().text()), Logger::Error);
		return;
	}

	if (!m_database.transaction())
		return;

	QSqlQuery addQuery(m_database);
	addQuery.prepare("INSERT INTO tags (id, tag, ttype) VALUES (:id, :tag, :ttype)");

	for (const Tag &tag : tags)
	{
		QString type = tag.type().name();
		addQuery.bindValue(":id", tag.id());
		addQuery.bindValue(":tag", tag.text());
		addQuery.bindValue(":ttype", tagTypes.contains(type) ? tagTypes[type] : -1);
		if (!addQuery.exec())
		{
			log(QString("SQL error when adding tag: %1").arg(addQuery.lastError().text()), Logger::Error);
			return;
		}
	}

	if (!m_database.commit())
		return;

	m_count = -1;
}

QMap<QString, TagType> TagDatabaseSqlite::getTagTypes(QStringList tags) const
{
	QMap<QString, TagType> ret;

	// Generate SQL with escaped values as we can't use WHERE IN (?) with prepared statements
	QString sql = "SELECT tag, ttype FROM tags WHERE tag IN (";
	QSqlDriver *driver = m_database.driver();
	for (int i = 0; i < tags.count(); ++i)
	{
		QSqlField f;
		f.setType(QVariant::String);
		f.setValue(tags[i]);
		sql += (i > 0 ? "," : "") + driver->formatValue(f);
	}
	sql += ")";

	QSqlQuery query(m_database);
	query.setForwardOnly(true);
	if (!query.exec(sql))
	{
		log(QString("SQL error when getting tags: %1").arg(query.lastError().text()), Logger::Error);
		return ret;
	}

	int idTag = query.record().indexOf("tag");
	int idTtype = query.record().indexOf("ttype");
	while (query.next())
		ret.insert(query.value(idTag).toString(), m_tagTypes[query.value(idTtype).toInt()]);

	return ret;
}

int TagDatabaseSqlite::count() const
{
	if (m_count != -1)
		return m_count;

	QSqlQuery query(m_database);
	QString sql = "SELECT COUNT(*) FROM tags";
	if (!query.exec(sql) || !query.next())
	{
		log(QString("SQL error when getting tag count: %1").arg(query.lastError().text()), Logger::Error);
		return -1;
	}

	m_count = query.value(0).toInt();
	return m_count;
}
