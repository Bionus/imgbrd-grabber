#ifndef DOWNLOAD_QUERY_LOADER_H
#define DOWNLOAD_QUERY_LOADER_H

#include <QString>
#include <QList>
#include <QMap>


class Site;
class DownloadQueryImage;
class DownloadQueryGroup;

class DownloadQueryLoader
{
	public:
		static bool load(QString path, QList<DownloadQueryImage> &uniques, QList<DownloadQueryGroup> &batchs, const QMap<QString, Site *> &sites);
		static bool save(QString path, const QList<DownloadQueryImage> &uniques, const QList<DownloadQueryGroup> &batchs);
};

#endif // DOWNLOAD_QUERY_LOADER_H
