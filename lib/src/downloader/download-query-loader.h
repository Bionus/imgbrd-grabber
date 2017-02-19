#ifndef DOWNLOAD_QUERY_LOADER_H
#define DOWNLOAD_QUERY_LOADER_H

#include <QString>
#include "download-query-group.h"
#include "download-query-image.h"


class DownloadQueryLoader
{
	public:
		static bool load(QString path, QList<DownloadQueryImage> &uniques, QList<DownloadQueryGroup> &batchs, QMap<QString, Site *> &sites);
		static bool save(QString path, QList<DownloadQueryImage> &uniques, QList<DownloadQueryGroup> &batchs);
};

#endif // DOWNLOAD_QUERY_LOADER_H
