#ifndef BACKUP_H
#define BACKUP_H

#include <QString>


class Profile;

bool saveBackup(Profile *profile, const QString &filePath);

#endif // BACKUP_H
