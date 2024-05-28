#include "logger.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <stdexcept>
#include "functions.h"


void Logger::initialize()
{
	// Create the "logs/" directory if it doesn't exist
	const QDir logsDirectory = savePath(QStringLiteral("logs/"), false, true);
	if (!logsDirectory.exists()) {
		logsDirectory.mkpath(".");
	}

	// Clean-up old log files if we have too many
	const QFileInfoList list = logsDirectory.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks, QDir::Time | QDir::Reversed);
	if (list.count() > MAX_LOG_FILES) {
		for (int i = 0; i < (list.count() - MAX_LOG_FILES); i++) {
			QFile::remove(list[i].absoluteFilePath());
		}
	}

	// Generate a new file for each run
	const QString logFilename = QString("main_%1-%2.log")
		.arg(QDate::currentDate().toString("yyyy.MM.dd"))
		.arg(QTime::currentTime().toString("hh.mm.ss.zzz"));
	setLogFile(logsDirectory.filePath(logFilename));
}


void Logger::logToConsole()
{
	if (m_logFile.isOpen()) {
		m_logFile.close();
	}
	m_logFile.open(stdout, QIODevice::WriteOnly);
}


void Logger::setLogFile(const QString &path)
{
	if (m_logFile.isOpen()) {
		m_logFile.close();
	}

	m_logFile.setFileName(path);
	m_logFile.open(QFile::Append | QFile::Text | QFile::Truncate);
}
QString Logger::logFile() const
{ return m_logFile.fileName(); }

/**
 * Sets the minimum log level for which messages will not be ignored.
 */
void Logger::setLogLevel(LogLevel level)
{
	m_level = level;
}

/**
 * Sets the minimum log level for which messages will also be printed to the console.
 */
void Logger::setConsoleOutputLevel(LogLevel level)
{
	m_consoleOutputLevel = level;
}

/**
 * Sets whether an error should also cause the program to stop (all errors considered as fatal).
 */
void Logger::setExitOnError(bool val)
{
	m_exitOnError = val;
}


/**
 * Qt message handler that formats and redirects the message to Grabber's logger.
 */
void Logger::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
	static const QMap<QtMsgType, LogLevel> messageTypes
	{
		{ QtMsgType::QtDebugMsg, Logger::Debug },
		#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
			{ QtMsgType::QtInfoMsg, Logger::Info },
		#endif
		{ QtMsgType::QtWarningMsg, Logger::Warning },
		{ QtMsgType::QtCriticalMsg, Logger::Error },
		{ QtMsgType::QtFatalMsg, Logger::Error },
		{ QtMsgType::QtSystemMsg, Logger::Error },
	};

	QString label = QStringLiteral("[Qt]");
	QString category(context.category);
	if (!category.isEmpty()) {
		label += "[" + category + "]";
	}
	#if defined QT_MESSAGELOGCONTEXT && defined QT_DEBUG && 0
		label += QStringLiteral("[%1(%2)::%3]").arg(context.file).arg(context.line).arg(context.function);
	#endif

	LogLevel level = messageTypes[type];
	if (message == QStringLiteral("libpng warning: iCCP: known incorrect sRGB profile")) {
		level = Logger::Debug;
	}

	Logger::getInstance().log(QStringLiteral("%1 %2").arg(label, message), level);
}

/**
 * Qt message handler that ignores the message.
 */
void Logger::noMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
	Q_UNUSED(type)
	Q_UNUSED(context)
	Q_UNUSED(message)
}

/**
 * Installs a message handler for Qt internal logs (using qDebug, qWarning, etc.).
 * @param log If true, will send all Qt internal logs to the internal log function. If false, drop them.
 */
void Logger::setupMessageOutput(bool log)
{
	qInstallMessageHandler(log ? Logger::messageOutput : Logger::noMessageOutput);
}

/**
 * Append text in the log in a new line.
 * @param message The message to append.
 */
void Logger::log(const QString &message, LogLevel level)
{
	static QTextStream qStdErr(stderr);

	if (level < m_level) {
		return;
	}

	// Fallback to "main.log" if no log file is open
	if (!m_logFile.isOpen()) {
		setLogFile(savePath(QStringLiteral("main.log"), false, true));
	}

	static const QString timeFormat = QStringLiteral("hh:mm:ss.zzz");
	static const QStringList levels = QStringList()
		<< QStringLiteral("Debug")
		<< QStringLiteral("Info")
		<< QStringLiteral("Warning")
		<< QStringLiteral("Error");
	const QString &levelStr = levels[level];
	const QDateTime time = QDateTime::currentDateTime();
	const QString timeStr = time.toString(timeFormat);

	// Write ASCII log to file
	const QString strippedMsg("[" + timeStr + "][" + levelStr + "] " + stripTags(message));
	m_logFile.write((strippedMsg + "\n").toUtf8());
	m_logFile.flush();

	// Emit colored HTML log
	const QString msg("[" + timeStr + "][" + levelStr + "] " + message);
	emit newLog(msg);

	// Print the message to the console
	if (level >= m_consoleOutputLevel) {
		qStdErr << strippedMsg << Qt::endl;
	}

	if (m_exitOnError && level == Logger::LogLevel::Error) {
		throw std::runtime_error(message.toStdString());
	}
}

void Logger::logCommand(const QString &l)
{
	if (!m_fCommandsLog.isOpen()) {
		m_fCommandsLog.setFileName(savePath(QStringLiteral("commands.log"), false, true));
		m_fCommandsLog.open(QFile::Append | QFile::Text | QFile::Truncate);
	}

	m_fCommandsLog.write(QString(l + "\r\n").toUtf8());
	m_fCommandsLog.flush();
}

void Logger::logCommandSql(const QString &l)
{
	if (!m_fCommandsSqlLog.isOpen()) {
		m_fCommandsSqlLog.setFileName(savePath(QStringLiteral("commands.sql"), false, true));
		m_fCommandsSqlLog.open(QFile::Append | QFile::Text | QFile::Truncate);
	}

	m_fCommandsSqlLog.write(QString(l + "\r\n").toUtf8());
	m_fCommandsSqlLog.flush();
}

/**
 * Append text in the log at the end of the current line.
 * @param	l	The message to append.
 */
void Logger::logUpdate(const QString &l)
{
	Q_UNUSED(l)

	/*QDateTime date = _log.keys().at(_log.count()-1);
	QString message = _log.value(date)+l;
	_log.insert(date, message);
	_mainwindow->logShow();

	qDebug() << l;*/
}


void log(const QString &l, Logger::LogLevel level)
{
	LOG(l, level);
}
