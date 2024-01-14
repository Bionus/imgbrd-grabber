#include "site-add-cli-command.h"
#include <QDebug>
#include <utility>
#include "models/profile.h"
#include "models/site.h"
#include "models/site-factory.h"
#include "models/source-guesser.h"


SiteAddCliCommand::SiteAddCliCommand(QStringList arguments, Profile *profile, QObject *parent)
	: CliAction(std::move(arguments), parent), m_profile(profile)
{
	m_parser.addHelpOptionOnly();
	m_parser.setApplicationDescription("Add a new source");
	m_parser.setPrefix({ "source", "add" });

	m_parser.addPositionalArgument("url", "The URL of the source to add", "<url>");

	m_parser.addOption(QCommandLineOption("guess", "Automatically guess the source type"));
	m_parser.addOption(QCommandLineOption("type", "Source type", "source_type"));
}


bool SiteAddCliCommand::validate()
{
	// One and only one site can be added at a time
	if (m_parser.positionalArguments().length() != 1) {
		m_parser.showHelp();
		return false;
	}

	// You should either pass the site type or let the program guess, not both
	if (m_parser.isSet("guess") && m_parser.isSet("type")) {
		qWarning() << "You cannot use both --guess and --type at the same time";
		return false;
	}

	return true;
}

void SiteAddCliCommand::run()
{
	// Get parameters from the parser
	const QString url = m_parser.positionalArguments().first();
	const QString type = m_parser.value("type");

	// An already added site will log a warning but still return 0
	const QString domain = SiteFactory::getDomain(url);
	if (m_profile->getSites().contains(domain)) {
		qWarning() << "Site already added:" << domain;
		emit finished(0);
		return;
	}

	QTextStream stdOut(stdout);

	// If the type is already provided, it's easy to add the site
	if (!type.isEmpty()) {
		Source *source = m_profile->getSources().value(type, nullptr);
		if (source == nullptr) {
			qWarning() << "Site type not found:" << type;
			emit finished(1);
			return;
		}

		m_profile->addSite(SiteFactory::fromUrl(url, source, m_profile));
		stdOut << "Site added:" << url;
		emit finished(0);
		return;
	}

	// Otherwise, we need to guess which type this site is
	SourceGuesser sourceGuesser(url, m_profile->getSources().values(), this);
	// TODO: show a progress bar
	connect(&sourceGuesser, &SourceGuesser::finished, [&](Source *source) {
		if (source == nullptr) {
			qWarning() << "The source could not be guessed:" << url;
			emit finished(1);
			return;
		}

		m_profile->addSite(SiteFactory::fromUrl(url, source, m_profile));
		stdOut << "Site added:" << url;
		emit finished(0);
	});
	sourceGuesser.start();
}
