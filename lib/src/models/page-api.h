#ifndef PAGE_API_H
#define PAGE_API_H

#include <QMap>
#include <QNetworkReply>
#include "tags/tag.h"


class Page;
class Api;
class Profile;
class Image;
class Site;

class PageApi : public QObject
{
	Q_OBJECT

	public:
		enum LoadResult
		{
			Ok,
			Error
		};

		explicit PageApi(Page *parentPage, Profile *profile, Site *site, Api *api, const QStringList &tags = QStringList(), int page = 1, int limit = 25, const QStringList &postFiltering = QStringList(), bool smart = false, QObject *parent = Q_NULLPTR, int pool = 0, int lastPage = 0, qulonglong lastPageMinId = 0, qulonglong lastPageMaxId = 0);
		void			setLastPage(Page *page);
		void			load(bool rateLimit = false);
		void			loadTags();
		QList<QSharedPointer<Image>> images();
		bool			isImageCountSure();
		bool			isPageCountSure();
		int				imagesCount(bool guess = true);
		int				pagesCount(bool guess = true);
		QUrl			url();
		QString			source();
		QString			wiki();
		QList<Tag>		tags();
		QStringList		search();
		QStringList		errors();
		int				imagesPerPage();
		int				highLimit();
		int				page();
		int				pageImageCount();
		qulonglong		minId() const;
		qulonglong		maxId() const;
		void			setUrl(const QUrl &url);
		QUrl			nextPage();
		QUrl			prevPage();

	public slots:
		void parse();
		void parseTags();
		void abort();
		void abortTags();
		void clear();

	signals:
		void finishedLoading(PageApi*, LoadResult);
		void finishedLoadingTags(PageApi*);

	protected:
		bool addImage(QSharedPointer<Image> img);
		void parseNavigation(const QString &source);
		void updateUrls();
		void setImageCount(int count, bool sure);
		void setPageCount(int count, bool sure);

	private:
		Page			*m_parentPage;
		Profile			*m_profile;
		Site			*m_site;
		Api				*m_api;
		QStringList		m_search, m_postFiltering, m_errors;
		int				m_imagesPerPage, m_lastPage, m_page, m_blim, m_pool;
		qulonglong		m_lastPageMinId, m_lastPageMaxId;
		bool			m_smart, m_isAltPage;
		QString			m_format, m_source, m_wiki, m_originalUrl;
		QUrl			m_url, m_urlRegex, m_urlNextPage, m_urlPrevPage;
		QList<QSharedPointer<Image>>	m_images;
		QList<Tag>		m_tags;
		QNetworkReply	*m_reply, *m_replyTags;
		int				m_imagesCount, m_pagesCount, m_pageImageCount;
		bool			m_imagesCountSafe, m_pagesCountSafe;
};

#endif // PAGE_API_H
