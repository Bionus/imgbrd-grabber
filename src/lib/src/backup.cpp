#include "backup.h"
#include <QFile>
#include <QHash>
#include <QTemporaryDir>
#include "functions.h"
#include "logger.h"
#include "models/favorite.h"
#include "models/profile.h"
#include "utils/zip.h"
#include "reverse-search/reverse-search-engine.h"
#include "reverse-search/reverse-search-loader.h"


bool saveBackup(Profile *profile, const QString &filePath)
{
	QHash<QString, QString> files;

	// Common files
	static const QStringList backupFiles { "settings.ini", "favorites.json", "viewitlater.txt", "ignore.txt", "wordsc.txt", "blacklist.txt", "monitors.json", "restore.igl", "tabs.json", "history.json" };
	for (const QString &file : backupFiles) {
		files.insert(profile->getPath() + "/" + file, file);
	}

	// Favorite thumbnails
	for (const Favorite &fav : profile->getFavorites()) {
		const QString relPath = "thumbs/" + fav.getName(true) + ".png";
		const QString favPath = savePath(relPath);
		files.insert(favPath, relPath);
	}

	// Web services icons
	ReverseSearchLoader loader(profile->getSettings());
	for (const auto &rse : loader.getAllReverseSearchEngines()) {
		const QString file = "webservices/" + QString::number(rse.id()) + ".ico";
		files.insert(profile->getPath() + "/" + file, file);
	}

	// Filter non-existing files
	QHash<QString, QString> zipFiles;
	for (auto it = files.constBegin(); it != files.constEnd(); ++it) {
		if (QFile::exists(it.key())) {
			zipFiles.insert(it.key(), it.value());
		}
	}

	// Create the backup ZIP
	const bool ok = createZip(filePath, zipFiles);
	if (!ok) {
		log("Failed to create backup ZIP file", Logger::Error);
	}

	return ok;
}

bool loadBackup(Profile *profile, const QString &filePath)
{
	// Create a temporary directory to store the extracted backup
	QTemporaryDir tmpDir;
	if (!tmpDir.isValid()) {
		log("Failed to create temporary directory to extract backup file", Logger::Error);
		return false;
	}

	// Unzip file
	if (!unzipFile(filePath, tmpDir.path())) {
		log("Failed to extract backup ZIP file", Logger::Error);
		return false;
	}

	// TODO(Bionus): actually implement loading here
	Q_UNUSED(profile)

	return true;
}
