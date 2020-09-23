#ifndef SQL_WORKER_H
#define SQL_WORKER_H

#include <QString>
#include <QThread>
#include <QtSql/QSqlDatabase>


class QVariant;

class SqlWorker : public QThread
{
	Q_OBJECT

	public:
		SqlWorker(QString driver, QString host, QString user, QString password, QString database, QObject *parent = nullptr);
		bool connect();
		static QString escape(const QVariant &val);

	public slots:
		bool execute(const QString &sql);

	private:
		QString m_driver;
		QString m_host;
		QString m_user;
		QString m_password;
		QString m_database;

		QSqlDatabase m_db;
		bool m_enabled;
		bool m_started;
};

#endif // SQL_WORKER_H
