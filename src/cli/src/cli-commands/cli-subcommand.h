#ifndef CLI_SUBCOMMAND_H
#define CLI_SUBCOMMAND_H

#include <QObject>
#include <QStringList>
#include "cli-command.h"
#include "vendor/qcommandlinecommandparser/qcommandlinecommandparser.h"


class CliSubcommand : public CliCommand
{
	Q_OBJECT

	public:
		explicit CliSubcommand(QStringList arguments, QObject *parent = nullptr);

		bool validate() override;
		void run() override;
		int execute() override;

		virtual CliCommand *getCommand(const QString &command) = 0;

	protected:
		QCommandLineCommandParser m_parser;
		QStringList m_arguments;
};

#endif // CLI_SUBCOMMAND_H
