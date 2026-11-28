#include "models/metadata-database/metadata-database.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <utility>
#include "logger.h"


const QStringList MetadataDatabase::TagTypes { "artist", "copyright", "character", "model", "species", "meta", "general", "contributor", "photo_set", "lore" };


static QString toJson(const QStringList &list)
{
	return QString::fromUtf8(QJsonDocument(QJsonArray::fromStringList(list)).toJson(QJsonDocument::Compact));
}

static QStringList fromJson(const QString &json)
{
	QStringList ret;
	const QJsonArray array = QJsonDocument::fromJson(json.toUtf8()).array();
	for (const auto &value : array) {
		ret.append(value.toString());
	}
	return ret;
}


bool operator==(const MetadataDatabaseEntry &lhs, const MetadataDatabaseEntry &rhs)
{
	return lhs.md5 == rhs.md5
		&& lhs.website == rhs.website
		&& lhs.id == rhs.id
		&& lhs.rating == rhs.rating
		&& lhs.date == rhs.date
		&& lhs.tags == rhs.tags
		&& lhs.allTags == rhs.allTags;
}
bool operator!=(const MetadataDatabaseEntry &lhs, const MetadataDatabaseEntry &rhs)
{
	return !(lhs == rhs);
}


MetadataDatabase::MetadataDatabase(QString path)
	: m_path(std::move(path)), m_connectionName("Metadata database - " + m_path)
{}

MetadataDatabase::~MetadataDatabase()
{
	// Queries must be destroyed before removing the connection
	m_getQuery = QSqlQuery();
	m_existsQuery = QSqlQuery();
	m_saveQuery = QSqlQuery();
	m_deleteQuery = QSqlQuery();
	m_countQuery = QSqlQuery();

	if (m_database.isValid()) {
		m_database.close();
		m_database = QSqlDatabase();
		QSqlDatabase::removeDatabase(m_connectionName);
	}
}


bool MetadataDatabase::load()
{
	if (m_loaded) {
		return true;
	}

	m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
	m_database.setDatabaseName(m_path);
	if (!m_database.open()) {
		log(QStringLiteral("Could not open metadata database: %1").arg(m_database.lastError().text()), Logger::Error);
		return false;
	}

	// Create the DB schema if necessary
	QStringList tagColumns;
	for (const QString &type : TagTypes) {
		tagColumns.append("\"" + type + "\" TEXT");
	}
	const QString createSql = QStringLiteral("CREATE TABLE IF NOT EXISTS metadata ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"md5 CHAR(32) NOT NULL, "
		"website TEXT NOT NULL, "
		"item TEXT NOT NULL, "
		"rating TEXT, "
		"\"date\" TEXT, "
		"%1, "
		"tags TEXT, "
		"updated_at TEXT, "
		"UNIQUE(md5, website, item)"
		")").arg(tagColumns.join(", "));
	QSqlQuery createQuery(m_database);
	if (!createQuery.exec(createSql)) {
		log(QStringLiteral("Could not create metadata database schema: %1").arg(createQuery.lastError().text()), Logger::Error);
		return false;
	}

	// Add the columns missing from databases created by older versions
	const QSqlRecord existingColumns = m_database.record(QStringLiteral("metadata"));
	for (const QString &column : QStringList { "date" } + TagTypes) {
		if (existingColumns.contains(column)) {
			continue;
		}
		QSqlQuery alterQuery(m_database);
		if (!alterQuery.exec(QStringLiteral("ALTER TABLE metadata ADD COLUMN \"%1\" TEXT").arg(column))) {
			log(QStringLiteral("Could not add the %1 column to the metadata database: %2").arg(column, alterQuery.lastError().text()), Logger::Error);
			return false;
		}
	}

	// Add indexes for fast lookup
	QSqlQuery indexQuery(m_database);
	if (!indexQuery.exec(QStringLiteral("CREATE INDEX IF NOT EXISTS metadata_md5 ON metadata(md5)"))
		|| !indexQuery.exec(QStringLiteral("CREATE INDEX IF NOT EXISTS metadata_website_item ON metadata(website, item)"))) {
		log(QStringLiteral("Could not create metadata database index: %1").arg(indexQuery.lastError().text()), Logger::Error);
		return false;
	}

	// Prepare queries
	QStringList columns { "md5", "website", "item", "rating", "\"date\"" };
	for (const QString &type : TagTypes) {
		columns.append("\"" + type + "\"");
	}
	columns.append({ "tags", "updated_at" });
	static const QStringList uniqueKey { "md5", "website", "item" };
	QStringList placeholders;
	QStringList updates;
	for (const QString &column : columns) {
		placeholders.append("?");
		if (column == QLatin1String("\"date\"")) {
			// Don't lose a known date if it is missing from the new data
			updates.append(column + " = COALESCE(NULLIF(excluded." + column + ", ''), " + column + ")");
		} else if (!uniqueKey.contains(column)) {
			updates.append(column + " = excluded." + column);
		}
	}

	m_getQuery = QSqlQuery(m_database);
	m_getQuery.prepare(QStringLiteral("SELECT * FROM metadata WHERE md5 = ? ORDER BY id"));
	m_existsQuery = QSqlQuery(m_database);
	m_existsQuery.prepare(QStringLiteral("SELECT 1 FROM metadata WHERE website = ? AND item = ? LIMIT 1"));
	m_saveQuery = QSqlQuery(m_database);
	m_saveQuery.prepare(QStringLiteral("INSERT INTO metadata (%1) VALUES (%2) ON CONFLICT(md5, website, item) DO UPDATE SET %3").arg(columns.join(", "), placeholders.join(", "), updates.join(", ")));
	m_deleteQuery = QSqlQuery(m_database);
	m_deleteQuery.prepare(QStringLiteral("DELETE FROM metadata WHERE md5 = ?"));
	m_countQuery = QSqlQuery(m_database);
	m_countQuery.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM metadata"));

	m_loaded = true;
	return true;
}

bool MetadataDatabase::isLoaded() const
{
	return m_loaded;
}

bool MetadataDatabase::save(const MetadataDatabaseEntry &entry)
{
	if (entry.md5.isEmpty() || entry.website.isEmpty() || entry.id.isEmpty() || !load()) {
		return false;
	}

	int i = 0;
	m_saveQuery.bindValue(i++, entry.md5);
	m_saveQuery.bindValue(i++, entry.website);
	m_saveQuery.bindValue(i++, entry.id);
	m_saveQuery.bindValue(i++, entry.rating);
	m_saveQuery.bindValue(i++, entry.date.isValid() ? entry.date.toUTC().toString(Qt::ISODateWithMs) : QString());
	for (const QString &type : TagTypes) {
		m_saveQuery.bindValue(i++, toJson(entry.tags.value(type)));
	}
	m_saveQuery.bindValue(i++, toJson(entry.allTags));
	m_saveQuery.bindValue(i++, (entry.updatedAt.isValid() ? entry.updatedAt : QDateTime::currentDateTimeUtc()).toString(Qt::ISODate));

	if (!m_saveQuery.exec()) {
		log(QStringLiteral("Error saving metadata to the database: %1").arg(m_saveQuery.lastError().text()), Logger::Error);
		return false;
	}

	log(QStringLiteral("Saved metadata of MD5 %1 from %2 (%3)").arg(entry.md5, entry.website, entry.id), Logger::Debug);
	return true;
}

QList<MetadataDatabaseEntry> MetadataDatabase::get(const QString &md5)
{
	QList<MetadataDatabaseEntry> ret;
	if (md5.isEmpty() || !load()) {
		return ret;
	}

	m_getQuery.bindValue(0, md5);
	if (!m_getQuery.exec()) {
		log(QStringLiteral("Error getting metadata from the database: %1").arg(m_getQuery.lastError().text()), Logger::Error);
		return ret;
	}

	const QSqlRecord record = m_getQuery.record();
	while (m_getQuery.next()) {
		MetadataDatabaseEntry entry;
		entry.md5 = m_getQuery.value(record.indexOf("md5")).toString();
		entry.website = m_getQuery.value(record.indexOf("website")).toString();
		entry.id = m_getQuery.value(record.indexOf("item")).toString();
		entry.rating = m_getQuery.value(record.indexOf("rating")).toString();
		const QString date = m_getQuery.value(record.indexOf("date")).toString();
		if (!date.isEmpty()) {
			entry.date = QDateTime::fromString(date, Qt::ISODateWithMs);
		}
		for (const QString &type : TagTypes) {
			const QStringList tags = fromJson(m_getQuery.value(record.indexOf(type)).toString());
			if (!tags.isEmpty()) {
				entry.tags.insert(type, tags);
			}
		}
		entry.allTags = fromJson(m_getQuery.value(record.indexOf("tags")).toString());
		entry.updatedAt = QDateTime::fromString(m_getQuery.value(record.indexOf("updated_at")).toString(), Qt::ISODate);
		ret.append(entry);
	}

	return ret;
}

/**
 * Check whether the metadata of a given post was already stored.
 */
bool MetadataDatabase::exists(const QString &website, const QString &id)
{
	if (website.isEmpty() || id.isEmpty() || !load()) {
		return false;
	}

	m_existsQuery.bindValue(0, website);
	m_existsQuery.bindValue(1, id);
	if (!m_existsQuery.exec()) {
		log(QStringLiteral("Error checking metadata in the database: %1").arg(m_existsQuery.lastError().text()), Logger::Error);
		return false;
	}

	const bool found = m_existsQuery.next();
	m_existsQuery.finish();
	return found;
}

bool MetadataDatabase::remove(const QString &md5)
{
	if (!load()) {
		return false;
	}

	m_deleteQuery.bindValue(0, md5);
	if (!m_deleteQuery.exec()) {
		log(QStringLiteral("Error removing metadata from the database: %1").arg(m_deleteQuery.lastError().text()), Logger::Error);
		return false;
	}

	return true;
}

int MetadataDatabase::count()
{
	if (!load()) {
		return -1;
	}

	if (!m_countQuery.exec()) {
		log(QStringLiteral("Error counting entries in the metadata database: %1").arg(m_countQuery.lastError().text()), Logger::Error);
		return -1;
	}

	const int idVal = m_countQuery.record().indexOf("cnt");
	m_countQuery.next();
	return m_countQuery.value(idVal).toInt();
}
