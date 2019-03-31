#include "utils/md5-fix/md5-fix-worker.h"
#include <QDir>
#include "functions.h"


void Md5FixWorker::doWork(const QString &d, const QString &format, const QStringList &suffixes, bool force)
{
	QDir dir(d);

	// Get all files from the destination directory
	auto files = listFilesFromDirectory(dir, suffixes);
	emit maximumSet(files.count());

	int loaded = 0;
	int total = 0;

	// Parse all files
	for (const auto &file : files) {
		const QString fileName = file.first;
		const QString path = dir.absoluteFilePath(fileName);

		QString md5 = force
			? getFileMd5(path)
			: getFilenameMd5(fileName, format);

		if (!md5.isEmpty()) {
			emit md5Calculated(md5, path);
			loaded++;
		}
		total++;

		emit valueSet(total);
	}

	emit finished(loaded);
}
