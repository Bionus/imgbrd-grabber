#include <QProcess>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDir>
#include "commands.h"
#include "functions.h"
#include "models/filename.h"



Commands::Commands(Profile *profile)
	: m_profile(profile)
{
	QSettings *settings = profile->getSettings();

	settings->beginGroup("Exec");
		m_commandTagBefore = settings->value("tag_before").toString();
		m_commandImage = settings->value("image").toString();
		m_commandTagAfter = settings->value("tag_after", settings->value("tag").toString()).toString();
		settings->beginGroup("SQL");
			m_mysqlSettings.driver = settings->value("driver", "QMYSQL").toString();
			m_mysqlSettings.host = settings->value("host").toString();
			m_mysqlSettings.user = settings->value("user").toString();
			m_mysqlSettings.password = settings->value("password").toString();
			m_mysqlSettings.database = settings->value("database").toString();
			m_mysqlSettings.before = settings->value("before").toString();
			m_mysqlSettings.tagBefore = settings->value("tag_before").toString();
			m_mysqlSettings.image = settings->value("image").toString();
			m_mysqlSettings.tagAfter = settings->value("tag_after", settings->value("tag").toString()).toString();
			m_mysqlSettings.after = settings->value("after").toString();
		settings->endGroup();
	settings->endGroup();

	m_mysql = !m_mysqlSettings.host.isEmpty() && !m_mysqlSettings.user.isEmpty() && !m_mysqlSettings.database.isEmpty();
	m_started = false;
}

bool Commands::start()
{
	if (m_mysql && !m_started)
	{
		m_started = true;
		QSqlDatabase db = QSqlDatabase::addDatabase(m_mysqlSettings.driver);
		db.setHostName(m_mysqlSettings.host);
		db.setDatabaseName(m_mysqlSettings.database);
		db.setUserName(m_mysqlSettings.user);
		db.setPassword(m_mysqlSettings.password);
		if (!db.open())
		{
			log(QObject::tr("Erreur lors de l'initialisation des commandes: %1").arg(db.lastError().text()));
			return false;
		}
	}
	return true;
}

bool Commands::before()
{
	if (m_mysql && !m_mysqlSettings.before.isEmpty())
	{
		start();

		log(QObject::tr("Execution SQL de \"%1\"").arg(m_mysqlSettings.before));
		logCommandSql(m_mysqlSettings.before);

		QSqlQuery query;
		return query.exec(m_mysqlSettings.before);
	}

	return true;
}

bool Commands::image(const Image &img, QString path)
{
	// Normal commands
	if (!m_commandImage.isEmpty())
	{
		Filename fn(m_commandImage);
		QStringList execs = fn.path(img, m_profile, "", 0, false, false, false, false);

		for (QString exec : execs)
		{
			exec.replace("%path:nobackslash%", QDir::toNativeSeparators(path).replace("\\", "/"))
				.replace("%path%", QDir::toNativeSeparators(path));

			log(QObject::tr("Execution de \"%1\"").arg(exec));
			logCommand(exec);

			QProcess::execute(exec);
		}
	}

	// SQL commands
	if (m_mysql && !m_mysqlSettings.image.isEmpty())
	{
		start();

		Filename fn(m_mysqlSettings.image);
		QStringList execs = fn.path(img, m_profile, "", 0, false, false, false, false);

		for (QString exec : execs)
		{
			exec.replace("%path:nobackslash%", QDir::toNativeSeparators(path).replace("\\", "/"))
				.replace("%path%", QDir::toNativeSeparators(path));

			log(QObject::tr("Execution SQL de \"%1\"").arg(exec));
			logCommandSql(exec);

			QSqlQuery query;
			if (!query.exec(exec))
				return false;
		}
	}

	return true;
}

bool Commands::tag(const Image &img, Tag tag, bool after)
{
	QMap<QString, int> types;
	types["general"] = 0;
	types["artist"] = 1;
	types["general"] = 2;
	types["copyright"] = 3;
	types["character"] = 4;
	types["model"] = 5;
	types["photo_set"] = 6;
	QString original = QString(tag.text()).replace(" ", "_");

	QString command = after ? m_commandTagAfter : m_commandTagBefore;
	if (!command.isEmpty())
	{
		Filename fn(command);
		QStringList execs = fn.path(img, m_profile, "", 0, false, false, false, false);

		for (QString exec : execs)
		{
			exec.replace("%tag%", original)
				.replace("%original%", tag.text())
				.replace("%type%", tag.type())
				.replace("%number%", QString::number(types[tag.type()]));

			log(QObject::tr("Execution seule de \"%1\"").arg(exec));
			logCommand(exec);

			QProcess::execute(exec);
		}
	}

	QString commandSql = after ? m_mysqlSettings.tagAfter : m_mysqlSettings.tagBefore;
	if (m_mysql && !commandSql.isEmpty())
	{
		start();

		Filename fn(commandSql);
		QStringList execs = fn.path(img, m_profile, "", 0, false, false, false, false);

		for (QString exec : execs)
		{
			exec.replace("%tag%", original)
				.replace("%original%", tag.text())
				.replace("%type%", tag.type())
				.replace("%number%", QString::number(types[tag.type()]));

			log(QObject::tr("Execution SQL de \"%1\"").arg(exec));
			logCommandSql(exec);

			QSqlQuery query;
			if (!query.exec(exec))
				return false;
		}
	}

	return true;
}

bool Commands::after()
{
	if (m_mysql && !m_mysqlSettings.after.isEmpty())
	{
		start();

		log(QObject::tr("Execution SQL de \"%1\"").arg(m_mysqlSettings.after));
		logCommandSql(m_mysqlSettings.after);

		QSqlQuery query;
		return query.exec(m_mysqlSettings.after);
	}

	return true;
}
