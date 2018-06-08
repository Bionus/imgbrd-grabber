#ifndef DOWNLOAD_QUERY_LOADER_H
#define DOWNLOAD_QUERY_LOADER_H

#include <QList>
#include <QMap>
#include <QString>


class Site;
class DownloadQueryImage;
class DownloadQueryGroup;

class DownloadQueryLoader
{
	public:
		static bool load(const QString &path, QList<DownloadQueryImage> &uniques, QList<DownloadQueryGroup> &groups, const QMap<QString, Site*> &sites);
		static bool save(const QString &path, const QList<DownloadQueryImage> &uniques, const QList<DownloadQueryGroup> &groups);
};

#endif // DOWNLOAD_QUERY_LOADER_H
