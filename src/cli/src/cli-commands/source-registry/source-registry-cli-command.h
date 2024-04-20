#ifndef SOURCE_REGISTRY_CLI_COMMAND_H
#define SOURCE_REGISTRY_CLI_COMMAND_H

#include <QObject>
#include "../cli-subcommand.h"


class Profile;

class SourceRegistryCliCommand : public CliSubcommand
{
	Q_OBJECT

	public:
		explicit SourceRegistryCliCommand(QStringList arguments, Profile *profile, QObject *parent = nullptr);

		CliCommand *getCommand(const QString &command) override;

	private:
		Profile *m_profile;
};

#endif // SOURCE_REGISTRY_CLI_COMMAND_H
