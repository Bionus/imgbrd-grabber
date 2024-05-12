#include "source-registry-list-cli-command.h"
#include <QDebug>
#include <QSettings>
#include <utility>
#include "models/profile.h"
#include "models/source-registry.h"


SourceRegistryListCliCommand::SourceRegistryListCliCommand(QStringList arguments, Profile *profile, QObject *parent)
	: CliAction(std::move(arguments), parent), m_profile(profile)
{
	m_parser.addHelpOptionOnly();
	m_parser.setApplicationDescription("List source registries");
	m_parser.setPrefix({ "source-registry", "list" });
}


bool SourceRegistryListCliCommand::validate()
{
	// Nothing to validate
	return true;
}

void SourceRegistryListCliCommand::run()
{
	// Log a warning and early return if no source registries are found
	const QList<SourceRegistry*> &sourceRegistries = m_profile->getSourceRegistries();
	if (sourceRegistries.isEmpty()) {
		qWarning() << "No source registries found";
		emit finished(0);
		return;
	}

	// Print all source registries' urls
	QTextStream stdOut(stdout);
	for (SourceRegistry *sourceRegistry : sourceRegistries) {
		stdOut << sourceRegistry->jsonUrl() << Qt::endl;
	}

	emit finished(0);
}
