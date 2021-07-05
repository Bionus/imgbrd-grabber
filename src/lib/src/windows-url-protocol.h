#ifndef WINDOWS_URL_PROTOCOL_H
#define WINDOWS_URL_PROTOCOL_H

#include <QString>


bool protocolExists(const QString &protocol);
void protocolInstall(const QString &protocol, const QString &command);
void protocolUninstall(const QString &protocol);

#endif // WINDOWS_URL_PROTOCOL_H
