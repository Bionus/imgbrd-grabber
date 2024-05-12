#ifndef SITE_ADD_CLI_COMMAND_H
#define SITE_ADD_CLI_COMMAND_H

#include <QObject>
#include "../cli-action.h"


class Profile;

class SiteAddCliCommand : public CliAction
{
	Q_OBJECT

	public:
		explicit SiteAddCliCommand(QStringList arguments, Profile *profile, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	private:
		Profile *m_profile;
};

#endif // SITE_ADD_CLI_COMMAND_H
