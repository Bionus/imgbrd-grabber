#ifndef SOURCE_REGISTRY_ADD_CLI_COMMAND_H
#define SOURCE_REGISTRY_ADD_CLI_COMMAND_H

#include <QObject>
#include "../cli-action.h"


class Profile;

class SourceRegistryAddCliCommand : public CliAction
{
	Q_OBJECT

	public:
		explicit SourceRegistryAddCliCommand(QStringList arguments, Profile *profile, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	private:
		Profile *m_profile;
};

#endif // SOURCE_REGISTRY_ADD_CLI_COMMAND_H
