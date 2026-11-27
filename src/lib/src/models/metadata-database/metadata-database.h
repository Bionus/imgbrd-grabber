#ifndef METADATA_DATABASE_H
#define METADATA_DATABASE_H

#include <QDateTime>
#include <QList>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QStringList>


/**
 * Metadata of a single file (identified by its MD5) on a single source.
 */
struct MetadataDatabaseEntry
{
	QString md5;
	QString website; // The site URL, without protocol, such as "danbooru.donmai.us"
	QString websiteName; // The site name as set in the sources settings
	QString id; // The ID of the post on that website
	QString rating;
	QDateTime date; // The date the post was uploaded on that website
	QMap<QString, QStringList> tags; // Tags grouped by type
	QStringList allTags; // All tags, in their original order
	QDateTime updatedAt;
};

bool operator==(const MetadataDatabaseEntry &lhs, const MetadataDatabaseEntry &rhs);
bool operator!=(const MetadataDatabaseEntry &lhs, const MetadataDatabaseEntry &rhs);


/**
 * SQLite database storing the metadata of downloaded files across all the sources they were found on, using their MD5 as a common identifier.
 */
class MetadataDatabase
{
	public:
		/** Tag types having their own column in the database. Other types are only available in the "tags" column. */
		static const QStringList TagTypes;

		explicit MetadataDatabase(QString path);
		~MetadataDatabase();

		bool load();
		bool isLoaded() const;
		bool save(const MetadataDatabaseEntry &entry);
		QList<MetadataDatabaseEntry> get(const QString &md5);
		bool exists(const QString &website, const QString &id);
		bool remove(const QString &md5);
		int count();

	private:
		QString m_path;
		QString m_connectionName;
		QSqlDatabase m_database;
		bool m_loaded = false;
		QSqlQuery m_getQuery;
		QSqlQuery m_existsQuery;
		QSqlQuery m_saveQuery;
		QSqlQuery m_deleteQuery;
		QSqlQuery m_countQuery;
};

#endif // METADATA_DATABASE_H
