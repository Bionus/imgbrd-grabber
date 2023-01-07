#ifndef GET_TAGS_CLI_COMMAND_H
#define GET_TAGS_CLI_COMMAND_H

#include <QList>
#include <QString>
#include "cli-command.h"


class Printer;
class Profile;
class QObject;
class Site;

class GetTagsCliCommand : public CliCommand
{
	Q_OBJECT

	public:
		explicit GetTagsCliCommand(Profile *profile, Printer *printer, const QList<Site*> &sites, int page, int perPage, int max, int tagsMin, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	private:
		Profile *m_profile;
		Printer *m_printer;
		QList<Site*> m_sites;
		int m_page;
		int m_perPage;
		int m_max;
		int m_tagsMin;
};

#endif // GET_TAGS_CLI_COMMAND_H
