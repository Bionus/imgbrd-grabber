#ifndef PAGE_H
#define PAGE_H

#include <QList>
#include <QObject>
#include <QString>
#include "models/page-api.h"


class Image;
class Site;
class Api;
class Tag;
class Profile;

class Page : public QObject
{
	Q_OBJECT

	public:
		explicit Page(Profile *profile, Site *site, QList<Site*> sites, QStringList tags = QStringList(), int page = 1, int limit = 25, QStringList postFiltering = QStringList(), bool smart = false, QObject *parent = Q_NULLPTR, int pool = 0, int lastPage = 0, qulonglong lastPageMinId = 0, qulonglong lastPageMaxId = 0);
		~Page() override;
		void		setLastPage(Page *page);
		void		fallback(bool loadIfPossible = true);
		void		load(bool rateLimit = false);
		void		loadTags();
		QList<QSharedPointer<Image>> images();
		Site		*site();
		int			imagesCount(bool guess = true);
		int			pagesCount(bool guess = true);
		QUrl		url();
		QUrl		friendlyUrl();
		QString		source();
		QString		website();
		QString		wiki();
		QList<Tag>	tags();
		QStringList	search();
		QStringList	errors();
		int			imagesPerPage();
		int			highLimit();
		int			page();
		int			pageImageCount();
		qulonglong	minId() const;
		qulonglong	maxId() const;
		QUrl		nextPage();
		QUrl		prevPage();

	public slots:
		void abort();
		void abortTags();
		void clear();

	protected slots:
		void loadFinished(PageApi *api, PageApi::LoadResult status);
		void loadTagsFinished(PageApi *api);

	signals:
		void finishedLoading(Page*);
		void failedLoading(Page*);
		void finishedLoadingTags(Page*);

	private:
		Site			*m_site;
		int				m_currentApi;
		QList<Api*>		m_siteApis;
		QList<PageApi*>	m_pageApis;
		int				m_regexApi;
		QStringList		m_postFiltering, m_errors, m_search;
		int				m_imagesPerPage, m_lastPage, m_imagesCount, m_pagesCount, m_page, m_blim, m_pool;
		qulonglong		m_lastPageMinId, m_lastPageMaxId;
		bool			m_smart;
		QString			m_format, m_website, m_source, m_originalUrl;
};

#endif // PAGE_H
