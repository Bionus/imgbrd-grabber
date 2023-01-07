#ifndef CLI_COMMAND_H
#define CLI_COMMAND_H

#include <QObject>


class CliCommand : public QObject
{
	Q_OBJECT

	public:
		explicit CliCommand(QObject *parent = nullptr);

		virtual bool validate() = 0;
		virtual void run() = 0;

	signals:
		void finished(int code);
};

#endif // CLI_COMMAND_H
