#include "file-utils.h"
#include "logger.h"
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSaveFile>
#include <QString>
#include <QStringList>


bool copyRecursively(QString srcFilePath, QString tgtFilePath, bool overwrite)
{
	// Trim directory names of their trailing slashes
	if (srcFilePath.endsWith(QDir::separator())) {
		srcFilePath.chop(1);
	}
	if (tgtFilePath.endsWith(QDir::separator())) {
		tgtFilePath.chop(1);
	}

	// Directly copy files using Qt function
	if (!QFileInfo(srcFilePath).isDir()) {
		if (QFile::exists(tgtFilePath)) {
			if (overwrite) {
				QFile::remove(tgtFilePath);
			} else {
				return false;
			}
		}
		return QFile(srcFilePath).copy(tgtFilePath);
	}

	// Try to create the target directory
	QDir targetDir(tgtFilePath);
	targetDir.cdUp();
	if (!targetDir.mkpath(QDir(tgtFilePath).dirName())) {
		return false;
	}

	QDir sourceDir(srcFilePath);
	const QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
	for (const QString &fileName : fileNames) {
		const QString newSrcFilePath = srcFilePath + QDir::separator() + fileName;
		const QString newTgtFilePath = tgtFilePath + QDir::separator() + fileName;
		if (!copyRecursively(newSrcFilePath, newTgtFilePath, overwrite)) {
			return false;
		}
	}

	return true;
}

bool safeCopyFile(const QString &from, const QString &filePath, bool backup)
{
	// Copy the file to a "bak" file to ensure no data is lost
	const QString backupFilePath = filePath + ".bak";
	if (QFile::exists(filePath)) {
		if (QFile::exists(backupFilePath) && !QFile::remove(backupFilePath)) {
			return false;
		}
		if (!QFile::rename(filePath, backupFilePath)) {
			return false;
		}
	}

	// Try to copy the file, otherwise restore the backup
	if (!QFile::copy(from, filePath)) {
		QFile::rename(backupFilePath, filePath);
		return false;
	}

	// Clean-up backup file
	if (!backup) {
		QFile::remove(backupFilePath);
	}
	return true;
}

bool safeWriteFile(const QString &filePath, const QByteArray &data, bool backup)
{
	// Copy the file to a "bak" file to ensure no data is lost
	if (backup) {
		const QString backupFilePath = filePath + ".bak";
		if (QFile::exists(backupFilePath) && !QFile::remove(backupFilePath)) {
			return false;
		}
		if (!QFile::copy(filePath, backupFilePath)) {
			return false;
		}
	}

	// Use QSaveFile to safely write data to the file
	QSaveFile file(filePath);
	if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		return false;
	}
	file.write(data);
	return file.commit();
}

bool ensureFileParent(const QString &filePath)
{
	const QString parentDir = QFileInfo(filePath).absolutePath();
	if (QDir().exists(parentDir)) {
		return true;
	}
	return QDir().mkpath(parentDir);
}

bool writeFile(const QString &filePath, const QByteArray &data)
{
	// Ensure the parent directory exists
	if (!ensureFileParent(filePath)) {
		log(QStringLiteral("Could not create parent directory for file `%1`").arg(filePath), Logger::Error);
		return false;
	}

	// Write the data to the disk
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
		log(QStringLiteral("Could not open file `%1` for writing: error %2 (%3)").arg(filePath, file.errorString(), QString::number(file.error())), Logger::Error);
		return false;
	}
	file.write(data);
	file.close();

	return true;
}

QString diagnoseDirectoryCreationError(const QString &dir)
{
	const QString cleaned = QDir::cleanPath(dir);

	// Walk up the tree to find the deepest existing ancestor
	QString existing = cleaned;
	QStringList missing;
	while (!existing.isEmpty() && !QFileInfo::exists(existing)) {
		const QFileInfo info(existing);
		const QString parent = info.path();
		if (parent == existing) {
			break;
		}
		missing.prepend(info.fileName());
		existing = parent;
	}

	const QFileInfo existingInfo(existing);

	// No part of the path exists at all (invalid drive, unreachable network share...)
	if (!existingInfo.exists()) {
		return QStringLiteral("no part of the path exists (invalid drive or unreachable location)");
	}

	// A file with the same name as one of the path components blocks the directory creation
	if (!existingInfo.isDir()) {
		return QStringLiteral("`%1` already exists and is a file, not a folder").arg(existing);
	}

	// The deepest existing ancestor is not writable
	if (!missing.isEmpty() && !existingInfo.isWritable()) {
		return QStringLiteral("the folder `%1` is not writable").arg(existing);
	}

	// Check for folder names that are not valid on Windows
	#ifdef Q_OS_WIN
		static const QStringList reserved { "CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9" };
		static const QRegularExpression forbiddenChars(QStringLiteral("[<>:\"|?*\\\\]"));
		for (const QString &part : missing) {
			if (part.endsWith('.') || part != part.trimmed() || part.contains(forbiddenChars) || reserved.contains(part.section('.', 0, 0), Qt::CaseInsensitive)) {
				return QStringLiteral("`%1` is not a valid folder name on Windows").arg(part);
			}
		}
	#endif

	return QString();
}

bool ensureDirectoryExists(const QString &dir)
{
	if (dir.isEmpty()) {
		log(QStringLiteral("Impossible to create the destination folder: empty path."), Logger::Error);
		return false;
	}

	if (QDir(dir).exists() || QDir().mkpath(dir)) {
		return true;
	}

	const QString reason = diagnoseDirectoryCreationError(dir);
	if (!reason.isEmpty()) {
		log(QStringLiteral("Impossible to create the destination folder: %1 (%2).").arg(dir, reason), Logger::Error);
	} else {
		log(QStringLiteral("Impossible to create the destination folder: %1.").arg(dir), Logger::Error);
	}
	return false;
}
