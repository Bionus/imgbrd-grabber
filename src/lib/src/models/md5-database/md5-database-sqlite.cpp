#include "models/md5-database/md5-database-sqlite.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <utility>
#include "logger.h"


Md5DatabaseSqlite::Md5DatabaseSqlite(QString path, QSettings *settings)
	: Md5Database(settings), m_path(std::move(path))
{
	// Use SQLite database for tests
	m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), "MD5 database - " + m_path);
	m_database.setDatabaseName(m_path);
	if (!m_database.open()) {
		log(QStringLiteral("Could not open MD5 database: %1").arg(m_database.lastError().text()), Logger::Error);
		return;
	}

	// Create schema if necessary
	QSqlQuery createQuery(QStringLiteral("CREATE TABLE IF NOT EXISTS md5s (md5 CHAR(32), path TEXT)"), m_database);
	if (!createQuery.exec()) {
		log(QStringLiteral("Could not create MD5 database schema: %1").arg(createQuery.lastError().text()), Logger::Error);
		return;
	}

	// Add index for fast lookup
	QSqlQuery indexQuery(QStringLiteral("CREATE INDEX IF NOT EXISTS md5s_md5 ON md5s(md5)"), m_database);
	if (!indexQuery.exec()) {
		log(QStringLiteral("Could not create MD5 database index: %1").arg(indexQuery.lastError().text()), Logger::Error);
		return;
	}

	// Prepare queries
	m_getQuery = QSqlQuery(m_database);
	m_getQuery.prepare(QStringLiteral("SELECT path FROM md5s WHERE md5 = :md5"));
	m_addQuery = QSqlQuery(m_database);
	m_addQuery.prepare(QStringLiteral("INSERT INTO md5s (md5, path) VALUES (:md5, :path)"));
	m_deleteQuery = QSqlQuery(m_database);
	m_deleteQuery.prepare(QStringLiteral("DELETE FROM md5s WHERE md5 = :md5 AND path = :path"));
	m_deleteAllQuery = QSqlQuery(m_database);
	m_deleteAllQuery.prepare(QStringLiteral("DELETE FROM md5s WHERE md5 = :md5"));
	m_countQuery = QSqlQuery(m_database);
	m_countQuery.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM md5s"));
}

Md5DatabaseSqlite::~Md5DatabaseSqlite()
{
	m_database.close();
}


void Md5DatabaseSqlite::sync()
{
	// No-op
}

void Md5DatabaseSqlite::add(const QString &md5, const QString &path)
{
	if (md5.isEmpty() || paths(md5).contains(path)) {
		return;
	}

	m_addQuery.bindValue(":md5", md5);
	m_addQuery.bindValue(":path", path);

	if (!m_addQuery.exec()) {
		log(QStringLiteral("Error adding MD5 to the database: %1").arg(m_addQuery.lastError().text()), Logger::Error);
	} else {
		log(QString("Added MD5: %1").arg(md5), Logger::Debug);
	}
}

void Md5DatabaseSqlite::remove(const QString &md5, const QString &path)
{
	QSqlQuery &query = path.isEmpty() ? m_deleteAllQuery : m_deleteQuery;

	query.bindValue(":md5", md5);
	if (!path.isEmpty()) {
		query.bindValue(":path", path);
	}

	if (!query.exec()) {
		log(QStringLiteral("Error removing MD5 from the database: %1").arg(query.lastError().text()), Logger::Error);
	}
}

QStringList Md5DatabaseSqlite::paths(const QString &md5)
{
	QStringList ret;

	m_getQuery.bindValue(":md5", md5);
	if (!m_getQuery.exec()) {
		log(QStringLiteral("Error getting MD5 from the database: %1").arg(m_getQuery.lastError().text()), Logger::Error);
		return ret;
	}

	int idVal = m_getQuery.record().indexOf("path");
	while (m_getQuery.next()) {
		ret.append(m_getQuery.value(idVal).toString());
	}

	return ret;
}

int Md5DatabaseSqlite::count() const
{
	if (!m_countQuery.exec()) {
		log(QStringLiteral("Error counting MD5s in the database: %1").arg(m_countQuery.lastError().text()), Logger::Error);
		return -1;
	}

	int idVal = m_countQuery.record().indexOf("cnt");
	m_countQuery.next();
	return m_countQuery.value(idVal).toInt();
}

void Md5DatabaseSqlite::setMd5s(const QMultiHash<QString, QString> &md5s)
{
	// Empty the database first
	QSqlQuery clearQuery(m_database);
	clearQuery.prepare(QStringLiteral("DELETE FROM md5s"));
	if (!clearQuery.exec()) {
		log(QStringLiteral("SQL error when clearing md5s: %1").arg(clearQuery.lastError().text()), Logger::Error);
		return;
	}

	bool transaction = false;
	int current = 0;

	for (auto it = md5s.constBegin(); it != md5s.constEnd(); ++it) {
		const QString &md5 = it.key();
		const QString &path = it.value();

		if (current % 500 == 0) {
			if (transaction) {
				if (!m_database.commit()) {
					log("Could not commit transaction", Logger::Error);
					return;
				}
			}
			if (!m_database.transaction()) {
				log("Could not create transaction", Logger::Error);
				return;
			}
			transaction = true;
		}

		m_addQuery.bindValue(":md5", md5);
		m_addQuery.bindValue(":path", path);
		if (!m_addQuery.exec()) {
			log(QStringLiteral("Error adding MD5 to the database: %1").arg(m_addQuery.lastError().text()), Logger::Error);
			continue;
		}

		current++;
	}

	if (transaction) {
		if (!m_database.commit()) {
			log("Could not commit transaction", Logger::Error);
			return;
		}
	}
}
