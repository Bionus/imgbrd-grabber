#include "source-registry-cli-command.h"
#include "source-registry-add-cli-command.h"
#include "source-registry-list-cli-command.h"
#include "source-registry-remove-cli-command.h"


SourceRegistryCliCommand::SourceRegistryCliCommand(QStringList arguments, Profile *profile, QObject *parent)
	: CliSubcommand(std::move(arguments), parent), m_profile(profile)
{
	m_parser.addHelpOptionOnly();
	m_parser.setApplicationDescription("Manage source registries");
	m_parser.setPrefix({ "source-registry" });

	m_parser.addCommand("list", "List source registries");
	m_parser.addCommand("add", "Add a new source registry");
	m_parser.addCommand("remove", "Remove an existing source registry");
}


CliCommand *SourceRegistryCliCommand::getCommand(const QString &command)
{
	if (command == "list") {
		return new SourceRegistryListCliCommand(m_arguments, m_profile, parent());
	}
	if (command == "add") {
		return new SourceRegistryAddCliCommand(m_arguments, m_profile, parent());
	}
	if (command == "remove") {
		return new SourceRegistryRemoveCliCommand(m_arguments, m_profile, parent());
	}
	return nullptr;
}
