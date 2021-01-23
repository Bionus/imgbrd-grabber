#ifndef GET_IMAGES_CLI_COMMAND_H
#define GET_IMAGES_CLI_COMMAND_H

#include <QList>
#include <QStringList>
#include "search-images-cli-command.h"


class Printer;
class Profile;
class QObject;
class Site;

class GetImagesCliCommand : public SearchImagesCliCommand
{
	Q_OBJECT

	public:
		explicit GetImagesCliCommand(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sites, int page, int perPage, QString filename, QString folder, int max, bool login, bool noDuplicates, bool getBlacklisted, bool loadMoreDetails, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	private:
		Printer *m_printer;
		bool m_loadMoreDetails;
};

#endif // GET_IMAGES_CLI_COMMAND_H
