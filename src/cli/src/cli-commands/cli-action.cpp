#include "cli-action.h"
#include <utility>


CliAction::CliAction(QStringList arguments, QObject *parent)
	: CliCommand(parent), m_arguments(std::move(arguments))
{}


int CliAction::execute()
{
	m_parser.process(m_arguments);

	return CliCommand::execute();
}
