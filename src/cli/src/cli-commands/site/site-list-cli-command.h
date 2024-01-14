#ifndef SITE_LIST_CLI_COMMAND_H
#define SITE_LIST_CLI_COMMAND_H

#include <QObject>
#include "../cli-action.h"


class Profile;

class SiteListCliCommand : public CliAction
{
	Q_OBJECT

	public:
		explicit SiteListCliCommand(QStringList arguments, Profile *profile, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	private:
		Profile *m_profile;
};

#endif // SITE_LIST_CLI_COMMAND_H
