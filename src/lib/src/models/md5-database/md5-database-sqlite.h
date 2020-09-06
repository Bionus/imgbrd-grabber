#ifndef MD5_DATABASE_SQLITE_H
#define MD5_DATABASE_SQLITE_H

#include "models/md5-database/md5-database.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QStringList>


class QSettings;

class Md5DatabaseSqlite : public Md5Database
{
	Q_OBJECT

	public:
		explicit Md5DatabaseSqlite(QString file, QSettings *settings);
		~Md5DatabaseSqlite() override;

		void sync() override;
		void add(const QString &md5, const QString &path) override;
		void remove(const QString &md5, const QString &path = {}) override;

	protected:
		QStringList paths(const QString &md5) override;

	private:
		QString m_path;
		QSqlDatabase m_database;
		QSqlQuery m_getQuery;
		QSqlQuery m_addQuery;
		QSqlQuery m_deleteQuery;
		QSqlQuery m_deleteAllQuery;
};

#endif // MD5_DATABASE_SQLITE_H
