#include "backup.h"
#include <QFile>
#include <QHash>
#include "functions.h"
#include "models/favorite.h"
#include "models/profile.h"
#include "utils/zip.h"


bool saveBackup(Profile *profile, const QString &filePath)
{
	// Common files
	static const QStringList backupFiles { "settings.ini", "favorites.json", "viewitlater.txt", "ignore.txt", "wordsc.txt", "blacklist.txt", "monitors.json", "restore.igl" };
	QHash<QString, QString> zipFiles;
	for (const QString &file : backupFiles) {
		zipFiles.insert(profile->getPath() + "/" + file, file);
	}

	// Favorite thumbnails
	for (const Favorite &fav : profile->getFavorites()) {
		const QString relPath = "thumbs/" + fav.getName(true) + ".png";
		const QString favPath = savePath(relPath);
		if (QFile::exists(favPath)) {
			zipFiles.insert(favPath, relPath);
		}
	}

	// Create backup ZIP
	return createZip(filePath, zipFiles);
}
