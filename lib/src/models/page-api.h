#ifndef PAGE_API_H
#define PAGE_API_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QUrl>
#include "models/filtering/post-filter.h"
#include "models/search-query/search-query.h"
#include "tags/tag.h"


class Api;
class Image;
class NetworkReply;
class Page;
class Profile;
class QTimer;
class SearchQuery;
class Site;

class PageApi : public QObject
{
	Q_OBJECT
	Q_ENUMS(LoadResult)

	public:
		enum LoadResult
		{
			Ok,
			Error
		};

		explicit PageApi(Page *parentPage, Profile *profile, Site *site, Api *api, SearchQuery query, int page = 1, int limit = 25, PostFilter postFiltering = PostFilter(), bool smart = false, QObject *parent = nullptr, int pool = 0, int lastPage = 0, qulonglong lastPageMinId = 0, qulonglong lastPageMaxId = 0);
		void setLastPage(Page *page);
		const QList<QSharedPointer<Image>> &images() const;
		bool isImageCountSure() const;
		bool isPageCountSure() const;
		int imagesCount(bool guess = true) const;
		int maxImagesCount() const;
		int pagesCount(bool guess = true) const;
		int maxPagesCount() const;
		const QUrl &url() const;
		const QString &source() const;
		const QString &wiki() const;
		const QList<Tag> &tags() const;
		const QStringList &errors() const;
		int imagesPerPage() const;
		int highLimit() const;
		bool hasNext() const;
		int page() const;
		int pageImageCount() const;
		qulonglong minId() const;
		qulonglong maxId() const;
		const QUrl &nextPage() const;
		const QUrl &prevPage() const;
		bool isLoaded() const;

	public slots:
		void load(bool rateLimit = false, bool force = false);
		void parse();
		void abort();
		void clear();

	signals:
		void finishedLoading(PageApi*, LoadResult);
		void finishedLoadingTags(PageApi*);
		void httpsRedirect();

	protected:
		bool addImage(const QSharedPointer<Image> &img);
		void updateUrls();
		void parseActual();
		void setImageCount(int count, bool sure);
		void setImageMaxCount(int maxCount);
		void setPageCount(int count, bool sure);
		void setReply(NetworkReply *reply);

	private:
		Page *m_parentPage;
		Profile *m_profile;
		Site *m_site;
		Api *m_api;
		SearchQuery m_query;
		QStringList m_errors;
		PostFilter m_postFiltering;
		int m_imagesPerPage, m_lastPage, m_page, m_pool;
		qulonglong m_lastPageMinId, m_lastPageMaxId;
		bool m_smart, m_isAltPage;
		QString m_format, m_source, m_wiki, m_originalUrl;
		QUrl m_url, m_urlNextPage, m_urlPrevPage;
		QMap<QString, QString> m_headers;
		QList<QSharedPointer<Image>> m_images;
		QList<Tag> m_tags;
		NetworkReply *m_reply, *m_replyTags;
		int m_imagesCount, m_maxImagesCount, m_pagesCount, m_pageImageCount;
		bool m_imagesCountSafe, m_pagesCountSafe;
		bool m_loading = false;
		bool m_loaded = false;
};

Q_DECLARE_METATYPE(PageApi::LoadResult)

#endif // PAGE_API_H
