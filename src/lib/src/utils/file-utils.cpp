#include "file-utils.h"
#include "logger.h"
#include <QDir>
#include <QFileInfo>
#include <QSaveFile>
#include <QString>
#include <QStringList>
#include <filesystem>
#include <system_error>


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

#ifdef Q_OS_WIN
	// Adapted from Qt's QFSFileEnginePrivate::longFileName, using public APIs for the absolute path.
	// https://github.com/qt/qtbase/blob/v6.10.0/src/corelib/io/qfsfileengine_win.cpp#L48
	static QString longFileName(const QString &path)
	{
		QString absPath = QDir::toNativeSeparators(QFileInfo(path).absoluteFilePath());
		if (absPath.startsWith("\\\\?\\") || absPath.startsWith("\\\\.\\")) {
			return absPath;
		}
		QString prefix = "\\\\?\\";
		if (absPath.startsWith("\\\\")) {
			prefix.append("UNC\\");
			absPath.remove(0, 2);
		}
		return prefix + absPath;
	}
#endif

bool ensureDirectoryExists(const QString &dir)
{
	if (dir.isEmpty()) {
		log(QStringLiteral("Impossible to create the destination folder: empty path."), Logger::Error);
		return false;
	}

	// Native filesystem calls would silently truncate a path at a null character.
	if (dir.contains(QChar::Null)) {
		log(QStringLiteral("Impossible to create the destination folder: %1 (path contains a null character).").arg(dir), Logger::Error);
		return false;
	}

	if (QDir(dir).exists()) {
		return true;
	}

	const QFileInfo info(dir);
	QString reason;
	if (!info.isNativePath()) {
		// Android content URIs and Qt resources need their Qt file engine.
		if (QDir().mkpath(dir)) {
			return true;
		}
		reason = QStringLiteral("Qt file engine did not provide an error description");
	} else {
		#ifdef Q_OS_WIN
			const std::filesystem::path path(longFileName(dir).toStdWString());
		#else
			const std::filesystem::path path = info.filesystemFilePath();
		#endif
		std::error_code error;
		std::filesystem::create_directories(path, error);
		// An existing directory returns false without an error.
		if (!error) {
			return true;
		}
		reason = QStringLiteral("%1; %2:%3")
				   .arg(QString::fromLocal8Bit(error.message().c_str()), QString::fromLatin1(error.category().name()))
				   .arg(error.value());
	}

	log(QStringLiteral("Impossible to create the destination folder: %1 (%2).").arg(dir, reason), Logger::Error);
	return false;
}
