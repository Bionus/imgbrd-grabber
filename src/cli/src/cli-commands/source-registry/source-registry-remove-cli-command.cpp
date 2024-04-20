#include "source-registry-remove-cli-command.h"
#include <QDebug>
#include <utility>
#include "models/profile.h"
#include "models/source-registry.h"


SourceRegistryRemoveCliCommand::SourceRegistryRemoveCliCommand(QStringList arguments, Profile *profile, QObject *parent)
	: CliAction(std::move(arguments), parent), m_profile(profile)
{
	m_parser.addHelpOptionOnly();
	m_parser.setApplicationDescription("Remove an existing source registry");
	m_parser.setPrefix({ "source-registry", "remove" });

	m_parser.addPositionalArgument("url", "The URL of the source registry to remove", "<url>");
}


bool SourceRegistryRemoveCliCommand::validate()
{
	// One and only one source registry can be removed at a time
	if (m_parser.positionalArguments().length() != 1) {
		m_parser.showHelp();
		return false;
	}

	return true;
}

void SourceRegistryRemoveCliCommand::run()
{
	const QString url = m_parser.positionalArguments().first();

	// Find the correct source registry
	auto sourceRegistryIt = std::find_if(
		m_profile->getSourceRegistries().constBegin(),
		m_profile->getSourceRegistries().constEnd(),
		[&](SourceRegistry *registry) { return registry->jsonUrl() == url; }
	);
	if (sourceRegistryIt == m_profile->getSourceRegistries().constEnd()) {
		qWarning() << "The source registry was not found:" << url;
		emit finished(1);
		return;
	}

	// Actually remove the source registry
	QTextStream stdOut(stdout);
	m_profile->removeSourceRegistry(*sourceRegistryIt);
	stdOut << "Source registry removed: " << url << Qt::endl;
	emit finished(0);
}
