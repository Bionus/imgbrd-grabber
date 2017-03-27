#ifndef SQL_WORKER_H
#define SQL_WORKER_H

#include <QString>
#include <QThread>


class SqlWorker : public QThread
{
	Q_OBJECT

	public:
		SqlWorker(QString driver, QString host, QString user, QString password, QString database, QObject *parent = 0);
		bool connect();
		static QString escape(QString text);

	public slots:
		bool execute(QString sql);

	private:
		QString m_driver;
		QString m_host;
		QString m_user;
		QString m_password;
		QString m_database;

		bool m_enabled;
		bool m_started;
};

#endif // SQL_WORKER_H
