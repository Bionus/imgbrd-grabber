#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include "models/filtering/blacklist.h"
#include "models/image.h"
#include "tags/tag.h"


struct ImageSaveResult;
class Page;
class Printer;
class Profile;
class Site;

class Downloader : public QObject
{
	Q_OBJECT

	public:
		Downloader() = default;
		Downloader(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sources, int page, int max, int perPage, QString location, QString filename, QString user, QString password, bool blacklist, Blacklist blacklistedTags, bool noDuplicates, int tagsMin, bool loadMoreDetails = false, bool login = true);
		void setQuit(bool quit);

		void getPageCount();
		void getTags();
		void getPageTags();
		void getImages();
		void getUrls();

	signals:
		void finishedPageCount(int);
		void finishedTags(const QList<Tag> &);
		void finishedImages(const QList<QSharedPointer<Image>> &);
		void finishedImage(QSharedPointer<Image> image);
		void quit();

	protected:
		QList<QSharedPointer<Image>> getAllImages();
		QList<Page*> getAllPagesTags();

	private:
		Profile *m_profile;
		Printer *m_printer;
		Page *m_lastPage;
		QStringList m_tags, m_postFiltering;
		QList<Site*> m_sites;
		int m_page, m_max, m_perPage, m_waiting, m_ignored, m_duplicates, m_tagsMin;
		bool m_loadMoreDetails;
		QString m_location, m_filename, m_user, m_password;
		bool m_blacklist, m_noDuplicates;
		Blacklist m_blacklistedTags;
		bool m_quit, m_login;
};

#endif // DOWNLOADER_H
