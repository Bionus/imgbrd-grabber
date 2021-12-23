#include "file-utils.h"
#include <QDir>
#include <QFileInfo>
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
