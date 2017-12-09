#ifndef DANBOORU_DOWNLOADER_IMPORTER_H
#define DANBOORU_DOWNLOADER_IMPORTER_H

#include <QSettings>
#include <QString>


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
