#include "cli-subcommand.h"
#include <utility>


CliSubcommand::CliSubcommand(QStringList arguments, QObject *parent)
	: CliCommand(parent), m_arguments(std::move(arguments))
{}


bool CliSubcommand::validate()
{
	// No-op
	return true;
}

void CliSubcommand::run()
{
	// No-op
}

int CliSubcommand::execute()
{
	if (!validate()) {
		return 1;
	}

	// Get and execute the command
	m_parser.process(m_arguments);
	CliCommand *cmd = getCommand(m_parser.command());
	if (cmd != nullptr) {
		int ret = cmd->execute();
		cmd->deleteLater();
		return ret;
	}

	// If we're here, that means that no command was passed
	m_parser.showHelp();
	return 0;
}
