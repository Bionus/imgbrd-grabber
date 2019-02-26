#ifndef PACK_LOADER_H
#define PACK_LOADER_H

#include <QLinkedList>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include "downloader/download-query-group.h"


class Image;
class Page;
class Profile;
class Site;

class PackLoader : public QObject
{
	Q_OBJECT

	public:
		explicit PackLoader(Profile *profile, DownloadQueryGroup query, int packSize = 1000, QObject *parent = nullptr);
		const DownloadQueryGroup &query() const;
		int nextPackSize() const;
		bool start();
		void abort();
		bool hasNext() const;
		QList<QSharedPointer<Image>> next();

	signals:
		void finishedPage(Page *page);

	private:
		Profile *m_profile;
		Site *m_site;
		DownloadQueryGroup m_query;
		int m_packSize;
		int m_total = 0;
		QLinkedList<Page*> m_pendingPages;
		QLinkedList<Page*> m_pendingGalleries;
		QList<QSharedPointer<Image>> m_overflow;
		bool m_overflowGallery = false;
		bool m_overflowHasNext = false;
		bool m_abort = false;
};

#endif // PACK_LOADER_H
