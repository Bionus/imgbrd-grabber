#ifndef DOWNLOAD_IMAGES_CLI_COMMAND_H
#define DOWNLOAD_IMAGES_CLI_COMMAND_H

#include <QList>
#include <QStringList>
#include "models/filtering/blacklist.h"
#include "search-images-cli-command.h"


class Printer;
class Profile;
class QObject;
class Site;

class DownloadImagesCliCommand : public SearchImagesCliCommand
{
	Q_OBJECT

	public:
		explicit DownloadImagesCliCommand(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sites, int page, int perPage, QString filename, QString folder, int max, bool login, bool noDuplicates, bool getBlacklisted, Blacklist blacklistedTags, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	private:
		Printer *m_printer;
		Blacklist m_blacklistedTags;
};

#endif // DOWNLOAD_IMAGES_CLI_COMMAND_H
