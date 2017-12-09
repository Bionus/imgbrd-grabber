#ifndef LOGGER_H
#define LOGGER_H

#define LOG(a, b) Logger::getInstance().log((a), (b))
#define DONE() Logger::getInstance().logUpdate(" Done")

#include <QDateTime>
#include <QFile>
#include <QObject>
#include <QString>


class Logger : public QObject
{
	Q_OBJECT

	public:
		enum LogLevel
		{
			Debug = 0,
			Info = 1,
			Warning = 2,
			Error = 3
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
		void setLogLevel(LogLevel level);
		void log(QString, LogLevel type = Info);
		void logCommand(QString);
		void logCommandSql(QString);
		void logUpdate(QString);

	signals:
		void newLog(QString message);

	private:
		Logger() = default;
		QFile m_logFile, m_fCommandsLog, m_fCommandsSqlLog;
		LogLevel m_level = LogLevel::Info;
};


// Temporary shortcut for the macro
void log(QString l, Logger::LogLevel level = Logger::Info);

#endif // LOGGER_H
