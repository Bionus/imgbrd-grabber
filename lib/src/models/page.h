#ifndef PAGE_H
#define PAGE_H

#include <QList>
#include <QObject>
#include <QString>
#include "models/page-api.h"


class Api;
class Image;
class Profile;
class Site;
class Tag;

class Page : public QObject
{
	Q_OBJECT

	public:
		explicit Page(Profile *profile, Site *site, const QList<Site *> &sites, QStringList tags = QStringList(), int page = 1, int limit = 25, const QStringList &postFiltering = QStringList(), bool smart = false, QObject *parent = nullptr, int pool = 0, int lastPage = 0, qulonglong lastPageMinId = 0, qulonglong lastPageMaxId = 0);
		~Page() override;
		void		setLastPage(Page *page);
		void		fallback(bool loadIfPossible = true);
		void		load(bool rateLimit = false);
		void		loadTags();
		const QList<QSharedPointer<Image>> &images() const;
		Site		*site() const;
		int			imagesCount(bool guess = true) const;
		int			maxImagesCount() const;
		int			pagesCount(bool guess = true) const;
		int			maxPagesCount() const;
		const QUrl	&url() const;
		const QUrl	&friendlyUrl() const;
		bool		hasSource() const;
		const QString		&website() const;
		const QString		&wiki() const;
		const QList<Tag>	&tags() const;
		const QStringList	&search() const;
		const QStringList	&errors() const;
		int			imagesPerPage() const;
		int			highLimit() const;
		int			page() const;
		int			pageImageCount() const;
		qulonglong	minId() const;
		qulonglong	maxId() const;
		const QUrl	&nextPage() const;
		const QUrl	&prevPage() const;

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
		Site			*m_site;
		int				m_currentApi;
		QList<Api*>		m_siteApis;
		QList<PageApi*>	m_pageApis;
		int				m_regexApi;
		QStringList		m_errors, m_search;
		int				m_imagesPerPage, m_lastPage, m_imagesCount, m_pagesCount, m_page, m_pool;
		qulonglong		m_lastPageMinId, m_lastPageMaxId;
		bool			m_smart;
		QString			m_format, m_website, m_source, m_originalUrl;
};

#endif // PAGE_H
