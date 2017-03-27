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
		setLogFile(savePath("main.log"));

	QDateTime time = QDateTime::currentDateTime();
	m_logFile.write(QString("["+time.toString("hh:mm:ss.zzz")+"] "+stripTags(l)+"\n").toUtf8());
	m_logFile.flush();

	QString msg = (level == Error ? QObject::tr("<b>Error:</b> %1").arg(l) : (level == Warning ? QObject::tr("<b>Warning:</b> %1").arg(l) : (level == Notice ? QObject::tr("<b>Notice:</b> %1").arg(l) : l)));
	emit newLog(time, msg);

	#ifdef QT_DEBUG
		qDebug() << time.toString("hh:mm:ss.zzz") << l;
	#endif
}

void Logger::logCommand(QString l)
{
	if (!m_fCommandsLog.isOpen())
	{
		m_fCommandsLog.setFileName(savePath("commands.log"));
		m_fCommandsLog.open(QFile::Append | QFile::Text | QFile::Truncate);
	}

	m_fCommandsLog.write(QString(l+"\r\n").toUtf8());
	m_fCommandsLog.flush();
}

void Logger::logCommandSql(QString l)
{
	if (!m_fCommandsSqlLog.isOpen())
	{
		m_fCommandsSqlLog.setFileName(savePath("commands.sql"));
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
