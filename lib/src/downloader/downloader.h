#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QVariant>
#include "models/image.h"


class Page;

class Downloader : public QObject
{
	Q_OBJECT

	public:
		Downloader() = default;
		~Downloader() override;
		Downloader(Profile *profile, QStringList tags, QStringList postFiltering, QList<Site*> sources, int page, int max, int perPage, QString location, QString filename, QString user, QString password, bool blacklist, QStringList blacklistedTags, bool noDuplicates, int tagsMin, QString tagsFormat, Downloader *previous = nullptr);
		void setQuit(bool quit);
		void downloadImages(QList<QSharedPointer<Image>> images);
		void loadNext();
		void setData(QVariant data);
		void getPageCount();
		void getTags();
		void getPageTags();
		void getImages();
		void getUrls();
		QVariant getData() const;
		QList<Page*> getPages() const;
		QList<Site*> getSites() const;
		int ignoredCount() const;
		int duplicatesCount() const;
		int pagesCount() const;
		int imagesMax() const;
		Page *lastPage() const;

	signals:
		void finished(QNetworkReply*);
		void finishedPageCount(int);
		void finishedTags(QList<Tag>);
		void finishedPageTags(QList<Tag>);
		void finishedImages(QList<QSharedPointer<Image>>);
		void finishedImagesPage(Page *page);
		void finishedImage(QSharedPointer<Image> image);
		void finishedUrls(QStringList);
		void finishedUrlsPage(Page *page);
		void quit();

	public slots:
		void returnInt(int ret);
		void returnString(QString ret);
		void returnTagList(QList<Tag> tags);
		void returnStringList(QStringList ret);
		void finishedLoadingPageCount(Page *page);
		void finishedLoadingTags(QList<Tag> tags);
		void finishedLoadingPageTags(Page *page);
		void finishedLoadingImages(Page *page);
		void finishedLoadingUrls(Page *page);
		void finishedLoadingImage();
		void cancel();
		void clear();

	private:
		Profile *m_profile;
		Page *m_lastPage;
		QStringList m_tags, m_postfiltering;
		QList<Site*> m_sites;
		int m_page, m_max, m_perpage, m_waiting, m_ignored, m_duplicates, m_tagsmin;
		QString m_location, m_filename, m_user, m_password;
		bool m_blacklist, m_noduplicates;
		QString m_tagsformat;
		QStringList m_blacklistedTags;

		QList<Page*> m_pages, m_pagesC, m_pagesT, m_opages, m_opagesC, m_opagesT;
		QList<QSharedPointer<Image>> m_images, m_imagesDownloading;
		QList<QPair<Site*, int> > m_pagesP, m_opagesP;
		QList<Tag> m_results;
		QVariant m_data;
		bool m_cancelled, m_quit;
		Downloader *m_previous;
};

#endif // DOWNLOADER_H
