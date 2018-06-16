#ifndef PAGE_API_H
#define PAGE_API_H

#include "models/image.h"
#include "tags/tag.h"


class Page;
class Api;
class Profile;
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

		explicit PageApi(Page *parentPage, Profile *profile, Site *site, Api *api, const QStringList &tags = QStringList(), int page = 1, int limit = 25, const QStringList &postFiltering = QStringList(), bool smart = false, QObject *parent = Q_NULLPTR, int pool = 0, int lastPage = 0, qulonglong lastPageMinId = 0, qulonglong lastPageMaxId = 0);
		void			setLastPage(Page *page);
		const QList<QSharedPointer<Image>> &images() const;
		bool			isImageCountSure() const;
		bool			isPageCountSure() const;
		int				imagesCount(bool guess = true) const;
		int				pagesCount(bool guess = true) const;
		const QUrl			&url() const;
		const QString		&source() const;
		const QString		&wiki() const;
		const QList<Tag>	&tags() const;
		const QStringList	&search() const;
		const QStringList	&errors() const;
		int				imagesPerPage() const;
		int				highLimit() const;
		int				page() const;
		int				pageImageCount() const;
		qulonglong		minId() const;
		qulonglong		maxId() const;
		const QUrl		&nextPage() const;
		const QUrl		&prevPage() const;
		bool			isLoaded() const;

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
		bool addImage(QSharedPointer<Image> img);
		void updateUrls();
		void parseActual();
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
		bool			m_loaded = false;
};

Q_DECLARE_METATYPE(PageApi::LoadResult)

#endif // PAGE_API_H
