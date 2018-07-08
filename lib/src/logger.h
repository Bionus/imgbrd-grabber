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

		// Handlers for Qt log messages
		static void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& message);
		static void noMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& message);
		static void setupMessageOutput(bool log);

		void setLogFile(const QString &path);
		void setLogLevel(LogLevel level);
		void log(const QString &, LogLevel level = Info);
		void logCommand(const QString &);
		void logCommandSql(const QString &);
		void logUpdate(const QString &);

		QString logFile() const;

	signals:
		void newLog(const QString &message);

	private:
		Logger() = default;
		QFile m_logFile, m_fCommandsLog, m_fCommandsSqlLog;
		LogLevel m_level = LogLevel::Info;
};


// Temporary shortcut for the macro
void log(const QString &l, Logger::LogLevel level = Logger::Info);

#endif // LOGGER_H
