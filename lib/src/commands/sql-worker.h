#ifndef SQL_WORKER_H
#define SQL_WORKER_H

#include <QThread>
#include <QVariant>


class SqlWorker : public QThread
{
	Q_OBJECT

	public:
		SqlWorker(const QString &driver, const QString &host, const QString &user, const QString &password, const QString &database, QObject *parent = Q_NULLPTR);
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

		bool m_enabled;
		bool m_started;
};

#endif // SQL_WORKER_H
