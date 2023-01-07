#ifndef CLI_H
#define CLI_H

#include <QMap>
#include <QString>
#include <QStringList>


class Profile;
class QCoreApplication;

int parseAndRunCliArgs(QCoreApplication *app, Profile *profile, bool defaultToGui, QMap<QString, QString> &params, QStringList &positionalArgs);

#endif // CLI_H
