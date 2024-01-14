#include "site-cli-command.h"
#include "site-add-cli-command.h"
#include "site-list-cli-command.h"


SiteCliCommand::SiteCliCommand(QStringList arguments, Profile *profile, QObject *parent)
	: CliSubcommand(std::move(arguments), parent), m_profile(profile)
{
	m_parser.addHelpOptionOnly();
	m_parser.setApplicationDescription("Manage sources");
	m_parser.setPrefix({ "source" });

	m_parser.addCommand("list", "List sources");
	m_parser.addCommand("add", "Add a new source");
}


CliCommand *SiteCliCommand::getCommand(const QString &command)
{
	if (command == "list") {
		return new SiteListCliCommand(m_arguments, m_profile, parent());
	}
	if (command == "add") {
		return new SiteAddCliCommand(m_arguments, m_profile, parent());
	}
	return nullptr;
}
