#ifndef GET_PAGE_COUNT_CLI_COMMAND_H
#define GET_PAGE_COUNT_CLI_COMMAND_H

#include <QList>
#include <QStringList>
#include "search-cli-command.h"


class Printer;
class Profile;
class QObject;
class Site;

class GetPageCountCliCommand : public SearchCliCommand
{
	Q_OBJECT

	public:
		explicit GetPageCountCliCommand(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sources, int page, int perPage, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	private:
		Printer *m_printer;
};

#endif // GET_PAGE_COUNT_CLI_COMMAND_H
