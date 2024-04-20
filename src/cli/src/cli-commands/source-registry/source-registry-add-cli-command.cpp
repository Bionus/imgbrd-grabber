#include "source-registry-add-cli-command.h"
#include <QDebug>
#include <utility>
#include "models/profile.h"
#include "models/source-registry.h"


SourceRegistryAddCliCommand::SourceRegistryAddCliCommand(QStringList arguments, Profile *profile, QObject *parent)
	: CliAction(std::move(arguments), parent), m_profile(profile)
{
	m_parser.addHelpOptionOnly();
	m_parser.setApplicationDescription("Add a new source registry");
	m_parser.setPrefix({ "source-registry", "add" });

	m_parser.addPositionalArgument("url", "The URL of the source registry to add", "<url>");
}


bool SourceRegistryAddCliCommand::validate()
{
	// One and only one source registry can be added at a time
	if (m_parser.positionalArguments().length() != 1) {
		m_parser.showHelp();
		return false;
	}

	return true;
}

void SourceRegistryAddCliCommand::run()
{
	// Build a source registry instance using the positional arguments
	const QString url = m_parser.positionalArguments().first();
	auto *sourceRegistry = new SourceRegistry(url);

	// Try to load the source registry first to ensure it's valid
	connect(sourceRegistry, &SourceRegistry::loaded, [=](bool ok) {
		if (!ok) {
			qWarning() << "The source registry could not be added:" << url;
			sourceRegistry->deleteLater();
			emit finished(1);
			return;
		}

		QTextStream stdOut(stdout);
		m_profile->addSourceRegistry(sourceRegistry);
		stdOut << "Source registry added: " << url << Qt::endl;
		emit finished(0);
	});
	sourceRegistry->load();
}
