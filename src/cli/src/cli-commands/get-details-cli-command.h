#ifndef GET_DETAILS_CLI_COMMAND_H
#define GET_DETAILS_CLI_COMMAND_H

#include <QList>
#include <QString>
#include "cli-command.h"


class Image;
class Printer;
class Profile;
class QObject;
class Site;

class GetDetailsCliCommand : public CliCommand
{
	Q_OBJECT

	public:
		explicit GetDetailsCliCommand(Profile *profile, Printer *printer, const QList<Site*> &sites, const QString &pageUrl, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	protected slots:
		void finishedLoading();

	private:
		Profile *m_profile;
		Printer *m_printer;
		QList<Site*> m_sites;
		QString m_pageUrl;
		Image *m_image;
};

#endif // LOAD_TAG_DATABASE_CLI_COMMAND_H
