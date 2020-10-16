#ifndef MD5_DATABASE_SQLITE_H
#define MD5_DATABASE_SQLITE_H

#include "models/md5-database/md5-database.h"
#include <QMultiHash>
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
		int count() const override;

		void setMd5s(const QMultiHash<QString, QString> &md5s);

	protected:
		QStringList paths(const QString &md5) override;

	private:
		QString m_path;
		QSqlDatabase m_database;
		mutable QSqlQuery m_getQuery;
		mutable QSqlQuery m_addQuery;
		mutable QSqlQuery m_deleteQuery;
		mutable QSqlQuery m_deleteAllQuery;
		mutable QSqlQuery m_countQuery;
};

#endif // MD5_DATABASE_SQLITE_H
