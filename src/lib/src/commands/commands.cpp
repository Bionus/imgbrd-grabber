#include "commands/commands.h"
#include <QDir>
#include <QProcess>
#include <QSettings>
#include "commands/sql-worker.h"
#include "functions.h"
#include "logger.h"
#include "models/filename.h"
#include "models/image.h"
#include "models/profile.h"
#include "tags/tag.h"



Commands::Commands(Profile *profile)
	: m_profile(profile)
{
	QSettings *settings = profile->getSettings();

	m_dryRun = settings->value("Exec/dry_run", false).toBool();
	m_commandTagBefore = settings->value("Exec/tag_before").toString();
	m_commandImage = settings->value("Exec/image").toString();
	m_commandTagAfter = settings->value("Exec/tag_after", settings->value("Exec/tag").toString()).toString();

	m_mysqlSettings.before = settings->value("Exec/SQL/before").toString();
	m_mysqlSettings.tagBefore = settings->value("Exec/SQL/tag_before").toString();
	m_mysqlSettings.image = settings->value("Exec/SQL/image").toString();
	m_mysqlSettings.tagAfter = settings->value("Exec/SQL/tag_after", settings->value("Exec/SQL/tag").toString()).toString();
	m_mysqlSettings.after = settings->value("Exec/SQL/after").toString();

	m_sqlWorker = new SqlWorker(
		settings->value("Exec/SQL/driver", "QMYSQL").toString(),
		settings->value("Exec/SQL/host").toString(),
		settings->value("Exec/SQL/user").toString(),
		settings->value("Exec/SQL/password").toString(),
		settings->value("Exec/SQL/database").toString(),
		settings->value("Exec/SQL/dry_run", false).toBool());
	m_sqlWorker->setObjectName("SqlThread");
}

Commands::~Commands()
{
	m_sqlWorker->deleteLater();
}

bool Commands::start() const
{
	return m_sqlWorker->connect();
}

bool Commands::before() const
{
	if (!m_mysqlSettings.before.isEmpty()) {
		return sqlExec(m_mysqlSettings.before);
	}

	return true;
}

bool Commands::image(const Image &img, const QString &path)
{
	// Normal commands
	if (!m_commandImage.isEmpty()) {
		Filename fn(m_commandImage);
		QStringList execs = fn.path(img, m_profile, QString(), 0, Filename::None);

		for (QString exec : execs) {
			exec.replace("%path:nobackslash%", QDir::toNativeSeparators(path).replace("\\", "/"))
				.replace("%path%", QDir::toNativeSeparators(path));

			if (!execute(exec)) {
				return false;
			}
		}
	}

	// SQL commands
	if (!m_mysqlSettings.image.isEmpty()) {
		Filename fn(m_mysqlSettings.image);
		fn.setEscapeMethod(&SqlWorker::escape);
		QStringList execs = fn.path(img, m_profile, QString(), 0, Filename::None);

		for (QString exec : execs) {
			exec.replace("%path:nobackslash%", m_sqlWorker->escape(QDir::toNativeSeparators(path).replace("\\", "/")))
				.replace("%path%", m_sqlWorker->escape(QDir::toNativeSeparators(path)));

			if (!sqlExec(exec)) {
				return false;
			}
		}
	}

	return true;
}

bool Commands::tag(const Image &img, const Tag &tag, bool after)
{
	const QString original = QString(tag.text()).replace(" ", "_");

	QString command = after ? m_commandTagAfter : m_commandTagBefore;
	if (!command.isEmpty()) {
		Filename fn(command);
		fn.setEscapeMethod(&SqlWorker::escape);
		QStringList execs = fn.path(img, m_profile, QString(), 0, Filename::KeepInvalidTokens);

		for (QString exec : execs) {
			exec.replace("%tag%", original)
				.replace("%original%", tag.text())
				.replace("%type%", tag.type().name())
				.replace("%number%", QString::number(tag.type().number(img.parentSite())));

			if (!execute(exec)) {
				return false;
			}
		}
	}

	QString commandSql = after ? m_mysqlSettings.tagAfter : m_mysqlSettings.tagBefore;
	if (!commandSql.isEmpty()) {
		start();

		Filename fn(commandSql);
		QStringList execs = fn.path(img, m_profile, QString(), 0, Filename::KeepInvalidTokens);

		for (QString exec : execs) {
			exec.replace("%tag%", m_sqlWorker->escape(original))
				.replace("%original%", m_sqlWorker->escape(tag.text()))
				.replace("%type%", m_sqlWorker->escape(tag.type().name()))
				.replace("%number%", QString::number(tag.type().number(img.parentSite())));

			if (!sqlExec(exec)) {
				return false;
			}
		}
	}

	return true;
}

bool Commands::after() const
{
	if (!m_mysqlSettings.after.isEmpty()) {
		return sqlExec(m_mysqlSettings.after);
	}

	return true;
}

bool Commands::execute(const QString &command) const
{
	#if defined(QT_NO_PROCESS)
		log(QStringLiteral("Cannot run commands on this platform (no QProcess"), Logger::Error);
		return false;
	#else
		log(QStringLiteral("Execution of \"%1\"").arg(command));
		Logger::getInstance().logCommand(command);

		if (m_dryRun) {
			return true;
		}

		QStringList args = splitCommand(command);
		QString program = args.takeFirst();

		QProcess proc;
		proc.start(program, args);

		// Connect command output to logs
		QObject::connect(&proc, &QProcess::readyReadStandardOutput, [&proc]() { log(QStringLiteral("[Command stdout] %1").arg(QString(proc.readAllStandardOutput())), Logger::Debug); });
		QObject::connect(&proc, &QProcess::readyReadStandardError, [&proc]() { log(QStringLiteral("[Command stderr] %1").arg(QString(proc.readAllStandardError())), Logger::Error); });

		// Wait for  the command to finish 30s
		if (!proc.waitForFinished()) {
			log(QStringLiteral("Command execution timeout"), Logger::Error);
		}

		// Check the return code for error codes (stderr output does not cause a "false" return)
		const int code = proc.exitCode();
		if (code != 0) {
			log(QStringLiteral("Error executing command (return code: %1)").arg(code), Logger::Error);
			return false;
		}

		return true;
	#endif
}

bool Commands::sqlExec(const QString &sql) const
{
	QMetaObject::invokeMethod(m_sqlWorker, "execute", Qt::QueuedConnection, Q_ARG(QString, sql));
	return true;
}
