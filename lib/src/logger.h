#ifndef LOGGER_H
#define LOGGER_H

#define LOG(a, b) Logger::getInstance().log((a), (b))
#define DONE() Logger::getInstance().logUpdate(" Done")

#include <QObject>
#include <QString>
#include <QFile>
#include <QDateTime>


class Logger : public QObject
{
	Q_OBJECT

	public:
		enum LogLevel
		{
			Debug,
			Info,
			Warning,
			Error
		};

		// Singleton pattern
		static Logger& getInstance()
		{
			static Logger instance;
			return instance;
		}
		Logger(Logger const&) = delete;
		void operator=(Logger const&) = delete;

		void setLogFile(QString path);
		void log(QString, LogLevel type = Info);
		void logCommand(QString);
		void logCommandSql(QString);
		void logUpdate(QString);

	signals:
		void newLog(QDateTime date, QString message);

	private:
		Logger() {}
		QFile m_logFile, m_fCommandsLog, m_fCommandsSqlLog;
};


// Temporary shortcut for the macro
void log(QString l, Logger::LogLevel level = Logger::Info);

#endif // LOGGER_H
