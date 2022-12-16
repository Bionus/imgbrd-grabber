#include "windows-url-protocol.h"
#include <QSettings>
#include "logger.h"


bool protocolExists(const QString &protocol)
{
	QSettings registry("HKEY_CURRENT_USER\\SOFTWARE\\Classes", QSettings::Registry64Format);
	return registry.childGroups().contains(protocol);
}

void protocolInstall(const QString &protocol, const QString &command)
{
	log(QStringLiteral("Installing URL protocol '%1' pointing to '%2'...").arg(protocol, command));

	QSettings registry("HKEY_CURRENT_USER\\SOFTWARE\\Classes", QSettings::Registry64Format);

	registry.beginGroup(protocol);
	registry.setValue("Default", QStringLiteral("URL:%1 Grabber protocol").arg(protocol));
	registry.setValue("URL Protocol", "");
	registry.setValue("shell/open/command/Default", command);
	registry.endGroup();
}

void protocolUninstall(const QString &protocol)
{
	log(QStringLiteral("Uninstalling URL protocol '%1'...").arg(protocol));

	QSettings registry("HKEY_CURRENT_USER\\SOFTWARE\\Classes", QSettings::Registry64Format);

	registry.beginGroup(protocol);
	registry.remove("");
	registry.endGroup();
}
