#include "commands/commands.h"
#include <QDir>
#include <QProcess>
#include "commands/sql-worker.h"
#include "functions.h"
#include "models/filename.h"
#include "models/profile.h"
#include "tags/tag.h"



Commands::Commands(Profile *profile)
	: m_profile(profile)
{
	QSettings *settings = profile->getSettings();

	settings->beginGroup("Exec");
		m_commandTagBefore = settings->value("tag_before").toString();
		m_commandImage = settings->value("image").toString();
		m_commandTagAfter = settings->value("tag_after", settings->value("tag").toString()).toString();
		settings->beginGroup("SQL");
			m_mysqlSettings.before = settings->value("before").toString();
			m_mysqlSettings.tagBefore = settings->value("tag_before").toString();
			m_mysqlSettings.image = settings->value("image").toString();
			m_mysqlSettings.tagAfter = settings->value("tag_after", settings->value("tag").toString()).toString();
			m_mysqlSettings.after = settings->value("after").toString();
		settings->endGroup();
	settings->endGroup();

	m_sqlWorker = new SqlWorker(settings->value("Exec/SQL/driver", "QMYSQL").toString(),
								settings->value("Exec/SQL/host").toString(),
								settings->value("Exec/SQL/user").toString(),
								settings->value("Exec/SQL/password").toString(),
								settings->value("Exec/SQL/database").toString());
	m_sqlWorker->setObjectName("SqlThread");
}

Commands::~Commands()
{
	m_sqlWorker->deleteLater();
}

bool Commands::start()
{
	return m_sqlWorker->connect();
}

bool Commands::before()
{
	if (!m_mysqlSettings.before.isEmpty())
		return sqlExec(m_mysqlSettings.before);

	return true;
}

bool Commands::image(const Image &img, const QString &path)
{
	// Normal commands
	if (!m_commandImage.isEmpty())
	{
		Filename fn(m_commandImage);
		QStringList execs = fn.path(img, m_profile, QString(), 0, false, false, false, false);

		for (QString exec : execs)
		{
			exec.replace("%path:nobackslash%", QDir::toNativeSeparators(path).replace("\\", "/"))
				.replace("%path%", QDir::toNativeSeparators(path));

			log(QStringLiteral("Execution of \"%1\"").arg(exec));
			Logger::getInstance().logCommand(exec);

			int code = QProcess::execute(exec);
			if (code != 0)
				log(QStringLiteral("Error executing command (return code: %1)").arg(code));
		}
	}

	// SQL commands
	if (!m_mysqlSettings.image.isEmpty())
	{
		Filename fn(m_mysqlSettings.image);
		fn.setEscapeMethod(&SqlWorker::escape);
		QStringList execs = fn.path(img, m_profile, QString(), 0, false, false, false, false);

		for (QString exec : execs)
		{
			exec.replace("%path:nobackslash%", m_sqlWorker->escape(QDir::toNativeSeparators(path).replace("\\", "/")))
				.replace("%path%", m_sqlWorker->escape(QDir::toNativeSeparators(path)));

			if (!sqlExec(exec))
				return false;
		}
	}

	return true;
}

bool Commands::tag(const Image &img, const Tag &tag, bool after)
{
	QString original = QString(tag.text()).replace(" ", "_");

	QString command = after ? m_commandTagAfter : m_commandTagBefore;
	if (!command.isEmpty())
	{
		Filename fn(command);
		fn.setEscapeMethod(&SqlWorker::escape);
		QStringList execs = fn.path(img, m_profile, QString(), 0, false, false, false, false, true);

		for (QString exec : execs)
		{
			exec.replace("%tag%", original)
				.replace("%original%", tag.text())
				.replace("%type%", tag.type().name())
				.replace("%number%", QString::number(tag.type().number()));

			log(QStringLiteral("Execution of \"%1\"").arg(exec));
			Logger::getInstance().logCommand(exec);

			int code = QProcess::execute(exec);
			if (code != 0)
				log(QStringLiteral("Error executing command (return code: %1)").arg(code));
		}
	}

	QString commandSql = after ? m_mysqlSettings.tagAfter : m_mysqlSettings.tagBefore;
	if (!commandSql.isEmpty())
	{
		start();

		Filename fn(commandSql);
		QStringList execs = fn.path(img, m_profile, QString(), 0, false, false, false, false, true);

		for (QString exec : execs)
		{
			exec.replace("%tag%", m_sqlWorker->escape(original))
				.replace("%original%", m_sqlWorker->escape(tag.text()))
				.replace("%type%", m_sqlWorker->escape(tag.type().name()))
				.replace("%number%", QString::number(tag.type().number()));

			if (!sqlExec(exec))
				return false;
		}
	}

	return true;
}

bool Commands::after()
{
	if (!m_mysqlSettings.after.isEmpty())
		return sqlExec(m_mysqlSettings.after);

	return true;
}

bool Commands::sqlExec(const QString &sql)
{
	QMetaObject::invokeMethod(m_sqlWorker, "execute", Qt::QueuedConnection, Q_ARG(QString, sql));
	return true;
}
