#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "page.h"
#include "image.h"



class Page;
class Image;

class Downloader : public QObject
{
	Q_OBJECT

	public:
		Downloader();
		Downloader(QStringList tags, QStringList postfiltering, QStringList sources, int page, int max, int perpage, QString location, QString filename, QString user, QString password, bool blacklist, bool noduplicates);
		void getPageCount();
		void getTags();
		void getImages();
		void setQuit(bool quit);

	signals:
		void finished(QNetworkReply*);
		void finishedPageCount(int);
		void finishedTags(QList<Tag>);
		void finishedImages(QStringList);

	public slots:
		void returnInt(int ret);
		void returnString(QString ret);
		void returnTagList(QList<Tag> ret);
		void returnStringList(QStringList ret);
		void finishedLoadingPageCount(Page *page);
		void finishedLoadingTags(Page *page);
		void finishedLoadingImages(Page *page);

	private:
		QList<Site*> *m_sites;
		QList<Page*> *m_pages;
		QStringList m_tags, m_postfiltering, m_sources;
		int m_page, m_max, m_perpage, m_waiting;
		QString m_location, m_filename, m_user, m_password;
		bool m_blacklist, m_noduplicates, m_quit;
};

#endif // DOWNLOADER_H
