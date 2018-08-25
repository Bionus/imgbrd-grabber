#ifndef PACK_LOADER_H
#define PACK_LOADER_H

#include <QList>
#include <QLinkedList>
#include <QSharedPointer>
#include "downloader/download-query-group.h"


class Downloadable;
class Page;
class Profile;
class Site;

class PackLoader
{
	public:
		explicit PackLoader(Profile *profile, DownloadQueryGroup query, int packSize = 1000);
		bool start();
		bool hasNext() const;
		QList<QSharedPointer<Downloadable>> next();

	private:
		Profile *m_profile;
		Site *m_site;
		DownloadQueryGroup m_query;
		int m_packSize;
		int m_total = 0;
		QLinkedList<Page*> m_pendingPages;
		QLinkedList<Page*> m_pendingGalleries;
};

#endif // PACK_LOADER_H
