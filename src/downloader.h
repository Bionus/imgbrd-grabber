#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "models/page.h"
#include "models/image.h"
#include <QVariant>



class Page;
class Image;

class Downloader : public QObject
{
	Q_OBJECT

	public:
		Downloader();
		~Downloader();
		Downloader(QStringList tags, QStringList postfiltering, QList<Site*> sources, int page, int max, int perpage, QString location, QString filename, QString user, QString password, bool blacklist, QStringList blacklistedtags, bool noduplicates, int tagsmin, QString tagsformat);
		void getPageCount();
		void getTags();
		void getPageTags();
		void getImages();
		void getUrls();
		void setQuit(bool quit);
		void downloadImages(QList<Image*> images);
		void loadNext();
        void setData(QVariant data);
        QVariant getData();
		QList<Page*> *getPages();
		QList<Site*> getSites();
		int ignoredCount();
		int duplicatesCount();
		int pagesCount();

	signals:
		void finished(QNetworkReply*);
		void finishedPageCount(int);
		void finishedTags(QList<Tag>);
		void finishedPageTags(QList<Tag>);
		void finishedImages(QList<Image*>);
        void finishedImagesPage(Page *page);
		void finishedImage(Image *image);
		void finishedUrls(QStringList);
        void finishedUrlsPage(Page *page);
		void quit();

	public slots:
		void returnInt(int ret);
		void returnString(QString ret);
		void returnTagList(QList<Tag> ret);
		void returnStringList(QStringList ret);
		void finishedLoadingPageCount(Page *page);
		void finishedLoadingTags(QList<Tag> tags);
		void finishedLoadingPageTags(Page *page);
		void finishedLoadingImages(Page *page);
		void finishedLoadingUrls(Page *page);
		void finishedLoadingImage(Image *image);
		void cancel();
		void clear();

	private:
		Page *m_lastPage;
		QStringList m_tags, m_postfiltering;
		QList<Site*> m_sites;
		int m_page, m_max, m_perpage, m_waiting, m_ignored, m_duplicates, m_tagsmin;
		QString m_location, m_filename, m_user, m_password;
		bool m_blacklist, m_noduplicates;
		QString m_tagsformat;
		QStringList m_blacklistedTags;

		QList<Page*> *m_pages, *m_pagesC, *m_pagesT, *m_opages, *m_opagesC, *m_opagesT;
		QList<Image*> *m_images;
		QList<QPair<Site*, int> > *m_pagesP, *m_opagesP;
		QList<Tag> *m_results;
        QVariant m_data;
        bool m_cancelled, m_quit;
};

#endif // DOWNLOADER_H
