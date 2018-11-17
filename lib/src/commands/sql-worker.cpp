#include "commands/sql-worker.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlField>
#include <QtSql/QSqlQuery>
#include "logger.h"


SqlWorker::SqlWorker(QString driver, QString host, QString user, QString password, QString database, QObject *parent)
	: QThread(parent), m_driver(std::move(driver)), m_host(std::move(host)), m_user(std::move(user)), m_password(std::move(password)), m_database(std::move(database))
{
	m_enabled = (m_driver == QLatin1String("QSQLITE") && !m_database.isEmpty())
		|| (!m_host.isEmpty() && !m_user.isEmpty() && !m_database.isEmpty());

	m_started = false;
}

bool SqlWorker::connect()
{
	if (!m_enabled || m_started)
		return true;

	QSqlDatabase db = QSqlDatabase::addDatabase(m_driver);
	db.setDatabaseName(m_database);
	db.setUserName(m_user);
	db.setPassword(m_password);

	const int portSeparator = m_host.lastIndexOf(':');
	if (portSeparator > 0)
	{
		db.setHostName(m_host.left(portSeparator));
		db.setPort(m_host.midRef(portSeparator + 1).toInt());
	}
	else
	{ db.setHostName(m_host); }

	if (!db.open())
	{
		log(QStringLiteral("Error initializing commands: %1").arg(db.lastError().text()), Logger::Error);
		return false;
	}

	m_started = true;
	return true;
}

QString SqlWorker::escape(const QVariant &val)
{
	QSqlDriver *driver = QSqlDatabase::database().driver();
	if (driver == nullptr)
		return nullptr;

	QSqlField f;
	f.setType(val.type());
	f.setValue(val);

	return driver->formatValue(f);
}

bool SqlWorker::execute(const QString &sql)
{
	if (!m_enabled || !connect())
		return false;

	log(QStringLiteral("SQL execution of \"%1\"").arg(sql));
	Logger::getInstance().logCommandSql(sql);

	QSqlQuery query;
	return query.exec(sql);
}
