#ifndef GET_PAGE_TAGS_CLI_COMMAND_H
#define GET_PAGE_TAGS_CLI_COMMAND_H

#include <QList>
#include <QStringList>
#include "search-cli-command.h"


class Printer;
class Profile;
class QObject;
class Site;

class GetPageTagsCliCommand : public SearchCliCommand
{
	Q_OBJECT

	public:
		explicit GetPageTagsCliCommand(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sources, int page, int perPage, int tagsMin, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	private:
		Printer *m_printer;
		int m_tagsMin;
};

#endif // GET_PAGE_TAGS_CLI_COMMAND_H
