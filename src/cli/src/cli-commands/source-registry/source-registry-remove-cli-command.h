#ifndef SOURCE_REGISTRY_REMOVE_CLI_COMMAND_H
#define SOURCE_REGISTRY_REMOVE_CLI_COMMAND_H

#include <QObject>
#include "../cli-action.h"


class Profile;

class SourceRegistryRemoveCliCommand : public CliAction
{
	Q_OBJECT

	public:
		explicit SourceRegistryRemoveCliCommand(QStringList arguments, Profile *profile, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	private:
		Profile *m_profile;
};

#endif // SOURCE_REGISTRY_REMOVE_CLI_COMMAND_H
