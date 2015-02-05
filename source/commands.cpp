#include <QProcess>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "commands.h"



Commands *Commands::_instance = NULL;
Commands* Commands::get()
{
	if (Commands::_instance == NULL)
	{ Commands::_instance = new Commands; }
	return Commands::_instance;
}

void Commands::init(QSettings *settings)
{
	settings->beginGroup("Exec");
		m_commandImage = settings->value("image").toString();
		m_commandTag = settings->value("tag").toString();
		settings->beginGroup("SQL");
			m_mysqlSettings.driver = settings->value("driver", "QMYSQL").toString();
			m_mysqlSettings.host = settings->value("host").toString();
			m_mysqlSettings.user = settings->value("user").toString();
			m_mysqlSettings.password = settings->value("password").toString();
			m_mysqlSettings.database = settings->value("database").toString();
			m_mysqlSettings.before = settings->value("before").toString();
			m_mysqlSettings.image = settings->value("image").toString();
			m_mysqlSettings.tag = settings->value("tag").toString();
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
			log(QObject::tr("Erreur lors de l'initialisation des commandes"));
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
bool Commands::image(Image *img, QString fp)
{
	if (!m_commandImage.isEmpty())
	{
		QStringList execs = img->path(m_commandImage, "", false);
		for (QString exec : execs)
		{
			exec.replace("%path%", fp);
			exec.replace(" \\C ", " /C ");
			log(QObject::tr("Execution de \"%1\"").arg(exec));
			logCommand(exec);
			QProcess::execute(exec);
		}
	}
	if (m_mysql && !m_mysqlSettings.image.isEmpty())
	{
		start();
		QStringList execs = img->path(m_mysqlSettings.image, "", false);
		for (QString exec : execs)
		{
			exec.replace("%path%", fp);
			log(QObject::tr("Execution SQL de \"%1\"").arg(exec));
			logCommandSql(exec);
			QSqlQuery query;
			if (!query.exec(exec))
				return false;
		}
	}
	return true;
}
bool Commands::tag(Tag tag)
{
	QMap<QString,int> types;
	types["general"] = 0;
	types["artist"] = 1;
	types["general"] = 2;
	types["copyright"] = 3;
	types["character"] = 4;
	types["model"] = 5;
	types["photo_set"] = 6;
	QString original = QString(tag.text()).replace(" ", "_");
	if (!m_commandTag.isEmpty())
	{
		QString exec = QString(m_commandTag)
		.replace("%tag%", original)
		.replace("%original%", tag.text())
		.replace("%type%", tag.type())
		.replace("%number%", QString::number(types[tag.type()]));
		log(QObject::tr("Execution seule de \"%1\"").arg(exec));
		logCommand(exec);
		QProcess::execute(exec);
	}
	if (m_mysql && !m_mysqlSettings.tag.isEmpty())
	{
		start();
		QString exec = QString(m_mysqlSettings.tag)
		.replace("%tag%", original)
		.replace("%original%", tag.text())
		.replace("%type%", tag.type())
		.replace("%number%", QString::number(types[tag.type()]));
		log(QObject::tr("Execution SQL de \"%1\"").arg(exec));
		logCommandSql(exec);
		QSqlQuery query;
		return query.exec(exec);
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
