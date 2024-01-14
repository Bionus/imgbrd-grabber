#ifndef SITE_CLI_COMMAND_H
#define SITE_CLI_COMMAND_H

#include <QObject>
#include "../cli-subcommand.h"


class Profile;

class SiteCliCommand : public CliSubcommand
{
	Q_OBJECT

	public:
		explicit SiteCliCommand(QStringList arguments, Profile *profile, QObject *parent = nullptr);

		CliCommand *getCommand(const QString &command) override;

	private:
		Profile *m_profile;
};

#endif // SITE_CLI_COMMAND_H
