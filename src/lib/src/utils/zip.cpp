#include "zip.h"
#include <QDir>
#include <vendor/miniz.h>
#include "logger.h"


bool createZip(const QString &filePath, const QHash<QString, QString> &files)
{
	// C-style initialization
	mz_zip_archive zip_archive;
	memset(&zip_archive, 0, sizeof(zip_archive));

	// Open archive
	log(QStringLiteral("Write zip to `%1`").arg(filePath), Logger::Info);
	mz_bool status = mz_zip_writer_init_file(&zip_archive, filePath.toStdString().c_str(), 0);
	if (!status) {
		log(QStringLiteral("mz_zip_writer_init_file failed"), Logger::Error);
		return false;
	}

	// Add all files to the archive
	for (auto it = files.constBegin(); it != files.constEnd(); ++it) {
		if (!mz_zip_writer_add_file(&zip_archive, it.value().toStdString().c_str(), it.key().toStdString().c_str(), NULL, 0, MZ_BEST_COMPRESSION)) {
			log(QStringLiteral("mz_zip_writer_add_file failed"), Logger::Error);
			return false;
		}
	}

	// Finalize and close the archive
	mz_zip_writer_finalize_archive(&zip_archive);
	mz_zip_writer_end(&zip_archive);

	return true;
}

bool unzipFile(const QString &filePath, const QString &destinationDir)
{
	QDir dir(destinationDir);
	if (!dir.exists() && !QDir().mkpath(destinationDir)) {
		log(QStringLiteral("Could not create ZIP extraction directory"), Logger::Error);
		return false;
	}

	// C-style initialization
	mz_zip_archive zip_archive;
	memset(&zip_archive, 0, sizeof(zip_archive));

	// Open archive
	log(QStringLiteral("Extracting zip `%1` to `%2`").arg(filePath, destinationDir), Logger::Info);
	mz_bool status = mz_zip_reader_init_file(&zip_archive, filePath.toStdString().c_str(), 0);
	if (!status) {
		log(QStringLiteral("mz_zip_reader_init_file failed"), Logger::Error);
		return false;
	}

	// Extract each file in the archive
	for (int i = 0; i < (int) mz_zip_reader_get_num_files(&zip_archive); i++) {
		// Read file information
		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
			log(QStringLiteral("mz_zip_reader_file_stat failed"), Logger::Error);
			mz_zip_reader_end(&zip_archive);
			return false;
		}

		// Extract the file
		if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
			dir.mkpath(QString(file_stat.m_filename));
		} else {
			mz_zip_reader_extract_file_to_file(&zip_archive, file_stat.m_filename, dir.filePath(file_stat.m_filename).toStdString().c_str(), 0);
		}
	}

	// Close the archive
	return mz_zip_reader_end(&zip_archive);
}
