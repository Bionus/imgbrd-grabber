#include "site-list-cli-command.h"
#include <QDebug>
#include <utility>
#include "models/profile.h"
#include "models/site.h"


SiteListCliCommand::SiteListCliCommand(QStringList arguments, Profile *profile, QObject *parent)
	: CliAction(std::move(arguments), parent), m_profile(profile)
{
	m_parser.addHelpOptionOnly();
	m_parser.setApplicationDescription("List sources");
	m_parser.setPrefix({ "source", "list" });
}


bool SiteListCliCommand::validate()
{
	// Nothing to validate
	return true;
}

void SiteListCliCommand::run()
{
	// Log a warning and early return if no sites are found
	const QMap<QString, Site*> &sites = m_profile->getSites();
	if (sites.isEmpty()) {
		qWarning() << "No sites found";
		emit finished(0);
		return;
	}

	// Print all sites' urls
	QTextStream stdOut(stdout);
	for (auto it = sites.constBegin(); it != sites.constEnd(); ++it) {
		stdOut << it.value()->url() << Qt::endl;
	}

	emit finished(0);
}
