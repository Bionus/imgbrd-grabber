#ifndef CLI_ACTION_H
#define CLI_ACTION_H

#include <QObject>
#include "cli-command.h"
#include "vendor/qcommandlinecommandparser/qcommandlinecommandparser.h"


class CliAction : public CliCommand
{
	Q_OBJECT

	public:
		explicit CliAction(QStringList arguments, QObject *parent = nullptr);

		int execute() override;

	protected:
		QCommandLineCommandParser m_parser;
		QStringList m_arguments;
};

#endif // CLI_ACTION_H
