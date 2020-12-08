#include "models/md5-database/md5-database.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>


Md5Database::Md5Database(QSettings *settings)
	: m_settings(settings)
{}


QPair<QString, QString> Md5Database::action(const QString &md5, const QString &target)
{
	// If the MD5 is not found, just save the image
	if (md5.isEmpty() || paths(md5).isEmpty()) {
		return { "save", "" };
	}

	// Get all existing paths for this MD5
	const QStringList pths = paths(md5);

	// Split paths into "same dir" and "not same dir"
	QStringList sameDirPaths, notSameDirPaths;
	const QDir targetDir = QFileInfo(target).dir();
	for (const QString &path : pths) {
		if (!target.isEmpty() && targetDir == QFileInfo(path).dir()) {
			sameDirPaths.append(path);
		} else {
			notSameDirPaths.append(path);
		}
	}

	// Prioritize paths in the same directory for the action
	if (!sameDirPaths.isEmpty()) {
		const QString setting = m_settings->value("Save/md5DuplicatesSameDir", "save").toString();
		const QPair<QString, QString> ret = action(md5, sameDirPaths, setting);
		if (!ret.first.isEmpty()) {
			return ret;
		}
	}

	// Fallback to duplicates found in other places
	if (!notSameDirPaths.isEmpty()) {
		const QString setting = m_settings->value("Save/md5Duplicates", "save").toString();
		const QPair<QString, QString> ret = action(md5, notSameDirPaths, setting);
		if (!ret.first.isEmpty()) {
			return ret;
		}
	}

	// Just save by default
	return { "save", "" };
}

QPair<QString, QString> Md5Database::action(const QString &md5, const QStringList &paths, QString action)
{
	// If one of the files already exists, return its path with the relevant action
	const bool keepDeleted = m_settings->value("Save/keepDeletedMd5", false).toBool();
	for (const QString &path : paths) {
		if (QFile::exists(path)) {
			return { action, path };
		} else if (!keepDeleted) {
			remove(md5, path);
		}
	}

	// If we want to keep deleted files, we can't return a "move" action or similar, as there are no files to move
	if (keepDeleted) {
		QString path = paths.first(); // The actual path is irrelevant since none exist
		if (action != "ignore") {
			action = "save";
		}
		return { action, path };
	}

	return { "", "" };
}

/**
 * Check if a file with this md5 already exists;
 * @param	md5		The md5 that needs to be checked.
 * @return			A QString containing the path to the already existing file, an empty QString if the md5 does not already exists.
 */
QStringList Md5Database::exists(const QString &md5)
{
	QStringList ret;

	if (md5.isEmpty()) {
		return ret;
	}


	const bool keepDeleted = m_settings->value("Save/keepDeletedMd5", false).toBool();

	const QStringList pths = paths(md5);
	for (const QString &path : pths) {
		if (QFile::exists(path) || keepDeleted) {
			ret.append(path);
		} else {
			remove(md5, path);
		}
	}

	return ret;
}
