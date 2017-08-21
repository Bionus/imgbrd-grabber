#include "logger.h"
#include "functions.h"

#ifdef QT_DEBUG
	#include <QDebug>
#endif


void Logger::setLogFile(QString path)
{
	m_logFile.setFileName(path);
	m_logFile.open(QFile::Append | QFile::Text | QFile::Truncate);
}

/**
 * Append text in the log in a new line.
 * @param	l	The message to append.
 */
void Logger::log(QString l, LogLevel level)
{
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
		msg = QString("<span style='color:%1'>%2</span>").arg(levelColor).arg(msg);
	emit newLog(msg);

	#ifdef QT_DEBUG
		qDebug() << time.toString("hh:mm:ss.zzz") << levelStr << l;
	#endif
}

void Logger::logCommand(QString l)
{
	if (!m_fCommandsLog.isOpen())
	{
		m_fCommandsLog.setFileName(savePath("commands.log", false, true));
		m_fCommandsLog.open(QFile::Append | QFile::Text | QFile::Truncate);
	}

	m_fCommandsLog.write(QString(l+"\r\n").toUtf8());
	m_fCommandsLog.flush();
}

void Logger::logCommandSql(QString l)
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
void Logger::logUpdate(QString l)
{
	Q_UNUSED(l);

	/*QDateTime date = _log.keys().at(_log.count()-1);
	QString message = _log.value(date)+l;
	_log.insert(date, message);
	_mainwindow->logShow();

	qDebug() << l;*/
}


void log(QString l, Logger::LogLevel level)
{
	LOG(l, level);
}
