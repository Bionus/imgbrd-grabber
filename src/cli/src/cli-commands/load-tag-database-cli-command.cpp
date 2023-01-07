#include "load-tag-database-cli-command.h"
#include <QList>
#include "cli-command.h"
#include "logger.h"
#include "models/site.h"
#include "tools/tag-list-loader.h"


LoadTagDatabaseCliCommand::LoadTagDatabaseCliCommand(Profile *profile, const QList<Site*> &sites, int minTagCount, QObject *parent)
	: CliCommand(parent), m_profile(profile), m_sites(sites), m_minTagCount(minTagCount)
{}

bool LoadTagDatabaseCliCommand::validate()
{
	if (m_sites.isEmpty()) {
		log("You must provide at least one source to load the tag database of", Logger::Error);
		return false;
	}

	if (m_minTagCount < 100) {
		log("Loading a tag database with a tag count under 100 can take a long time and generate lots of requests", Logger::Warning);
	}

	return true;
}

void LoadTagDatabaseCliCommand::run()
{
	loadNext();
}

void LoadTagDatabaseCliCommand::loadNext()
{
	Site *site = m_sites.takeFirst();

	auto *loader = new TagListLoader(m_profile, site, m_minTagCount, this);
	connect(loader, &TagListLoader::finished, this, &LoadTagDatabaseCliCommand::finishedLoading);
	connect(loader, &TagListLoader::finished, loader, &TagListLoader::deleteLater);
	loader->start();
}

void LoadTagDatabaseCliCommand::finishedLoading()
{
	if (!m_sites.isEmpty()) {
		loadNext();
		return;
	}

	emit finished(0);
}
