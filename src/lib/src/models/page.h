#ifndef PAGE_H
#define PAGE_H

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include "models/page-api.h"


class Api;
class Image;
class Profile;
class QDateTime;
class SearchQuery;
class Site;
class Tag;

class Page : public QObject
{
	Q_OBJECT

	public:
		explicit Page(Profile *profile, Site *site, const QList<Site*> &sites, SearchQuery query, int page = 1, int limit = 25, const QStringList &postFiltering = QStringList(), bool smart = false, QObject *parent = nullptr, int pool = 0, int lastPage = 0, qulonglong lastPageMinId = 0, qulonglong lastPageMaxId = 0, const QString& lastPageMinDate = "", const QString& lastPageMaxDate = "");
		~Page() override;
		void setLastPage(Page *page);
		void fallback(bool loadIfPossible = true);
		void load(bool rateLimit = false);
		void loadTags();
		const QList<QSharedPointer<Image>> &images() const;
		Site *site() const;
		int imagesCount(bool guess = true) const;
		int maxImagesCount() const;
		int pagesCount(bool guess = true) const;
		int maxPagesCount() const;
		const QUrl &url() const;
		const QUrl &friendlyUrl() const;
		bool hasSource() const;
		const QString &website() const;
		const QString &wiki() const;
		const QList<Tag> &tags() const;
		const SearchQuery &query() const;
		const QStringList &search() const;
		const QStringList &errors() const;
		int imagesPerPage() const;
		int highLimit() const;
		bool hasNext() const;
		int page() const;
		int pageImageCount() const;
		qulonglong minId() const;
		qulonglong maxId() const;
		QString minDate() const;
		QString maxDate() const;
		const QUrl &nextPage() const;
		const QUrl &prevPage() const;
		bool isLoaded() const;
		bool isValid() const;
		QMap<QString, QUrl> urls() const;

	public slots:
		void abort();
		void abortTags();
		void clear();

	protected slots:
		void loadFinished(PageApi *api, PageApi::LoadResult status);
		void loadTagsFinished(PageApi *api, PageApi::LoadResult status);
		void httpsRedirectSlot();

	signals:
		void finishedLoading(Page*);
		void failedLoading(Page*);
		void finishedLoadingTags(Page*);
		void httpsRedirect(Page*);

	private:
		Site *m_site;
		int m_currentApi;
		QList<Api*> m_siteApis;
		QList<PageApi*> m_pageApis;
		int m_regexApi;
		SearchQuery m_query;
		QStringList m_errors, m_search;
		int m_imagesPerPage, m_imagesCount, m_pagesCount, m_page, m_pool;
		bool m_smart;
		QString m_website;
};

#endif // PAGE_H
