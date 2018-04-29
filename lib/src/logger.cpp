#include "logger.h"
#include "functions.h"

#ifdef QT_DEBUG
	#include <QDebug>
#endif


void Logger::setLogFile(const QString &path)
{
	if (m_logFile.isOpen())
	{ m_logFile.close(); }

	m_logFile.setFileName(path);
	m_logFile.open(QFile::Append | QFile::Text | QFile::Truncate);
}
QString Logger::logFile() const
{ return m_logFile.fileName(); }

void Logger::setLogLevel(LogLevel level)
{
	m_level = level;
}


void Logger::messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	static QMap<QtMsgType, LogLevel> messageTypes
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

	QString label = "[Qt]";
	QString category(context.category);
	if (!category.isEmpty())
		label += "[" + category + "]";
	#if defined QT_MESSAGELOGCONTEXT && defined QT_DEBUG && 0
		label += QString("[%1(%2)::%3]").arg(context.file).arg(context.line).arg(context.function);
	#endif

	Logger::getInstance().log(QString("%1 %2").arg(label, message), messageTypes[type]);
}

void Logger::noMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	Q_UNUSED(type);
	Q_UNUSED(context);
	Q_UNUSED(message);
}

void Logger::setupMessageOutput(bool log)
{
	qInstallMessageHandler(log ? Logger::messageOutput : Logger::noMessageOutput);
}

/**
 * Append text in the log in a new line.
 * @param	l	The message to append.
 */
void Logger::log(const QString &l, LogLevel level)
{
	if (level < m_level)
		return;

	if (!m_logFile.isOpen())
		setLogFile(savePath("main.log", false, true));

	static const QStringList levels = QStringList() << "Debug" << "Info" << "Warning" << "Error";
	static const QStringList colors = QStringList() << "#999" << "" << "orange" << "red";
	QString levelStr = levels[level];
	QDateTime time = QDateTime::currentDateTime();

	// Write ASCII log to file
	m_logFile.write(QString("["+time.toString("hh:mm:ss.zzz")+"]["+levelStr+"] "+stripTags(l)+"\n").toUtf8());
	m_logFile.flush();

	// Emit colored HTML log
	QString levelColor = colors[level];
	QString msg = "[" + time.toString("hh:mm:ss.zzz") + "][" + levelStr + "] " + l;
	if (!levelColor.isEmpty())
		msg = QString("<span style='color:%1'>%2</span>").arg(levelColor, msg);
	emit newLog(msg);

	#ifdef QT_DEBUG
		qDebug() << time.toString("hh:mm:ss.zzz") << levelStr << l;
	#endif
}

void Logger::logCommand(const QString &l)
{
	if (!m_fCommandsLog.isOpen())
	{
		m_fCommandsLog.setFileName(savePath("commands.log", false, true));
		m_fCommandsLog.open(QFile::Append | QFile::Text | QFile::Truncate);
	}

	m_fCommandsLog.write(QString(l+"\r\n").toUtf8());
	m_fCommandsLog.flush();
}

void Logger::logCommandSql(const QString &l)
{
	if (!m_fCommandsSqlLog.isOpen())
	{
		m_fCommandsSqlLog.setFileName(savePath("commands.sql", false, true));
		m_fCommandsSqlLog.open(QFile::Append | QFile::Text | QFile::Truncate);
	}

	m_fCommandsSqlLog.write(QString(l+"\r\n").toUtf8());
	m_fCommandsSqlLog.flush();
}

/**
 * Append text in the log at the end of the current line.
 * @param	l	The message to append.
 */
void Logger::logUpdate(const QString &l)
{
	Q_UNUSED(l);

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
