#ifndef DANBOORU_DOWNLOADER_IMPORTER_H
#define DANBOORU_DOWNLOADER_IMPORTER_H

#include <QString>


class QSettings;

class DanbooruDownloaderImporter
{
	public:
		DanbooruDownloaderImporter();
		bool isInstalled() const;
		void import(QSettings *dest) const;

	private:
		QString m_firefoxProfilePath;
};

#endif // DANBOORU_DOWNLOADER_IMPORTER_H
