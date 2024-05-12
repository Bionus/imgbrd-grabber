#ifndef BACKUP_H
#define BACKUP_H

#include <QString>


class Profile;

/**
 * Generates a backup archive for the given profile.
 *
 * @param profile The profile to generate a backup for.
 * @param filePath The path of the destination ZIP file.
 * @return Whether the backup could be generated correctly.
 */
bool saveBackup(Profile *profile, const QString &filePath);

/**
 * Restore settings and data from the given backup archive.
 *
 * @param profile The profile in which the backup should be loaded.
 * @param filePath The path of the backup ZIP file.
 * @return Whether the backup could be restored correctly.
 */
bool loadBackup(Profile *profile, const QString &filePath);

#endif // BACKUP_H
