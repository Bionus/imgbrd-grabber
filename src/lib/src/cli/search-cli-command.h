#ifndef SEARCH_CLI_COMMAND_H
#define SEARCH_CLI_COMMAND_H

#include <QList>
#include <QStringList>
#include "cli-command.h"


class Page;
class Profile;
class QObject;
class Site;

class SearchCliCommand : public CliCommand
{
	Q_OBJECT

	public:
		explicit SearchCliCommand(Profile *profile, QStringList tags, QStringList postFiltering, QList<Site*> sites, int page, int perPage, QObject *parent = nullptr);

	protected:
		QList<Page*> getTagsForAllPages();

	protected:
		Profile *m_profile;
		QStringList m_tags;
		QStringList m_postFiltering;
		QList<Site*> m_sites;
		int m_page;
		int m_perPage;
};

#endif // SEARCH_CLI_COMMAND_H
