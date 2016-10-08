#ifndef PAGE_API_H
#define PAGE_API_H

#include <QMap>
#include <QNetworkReply>
#include <QSslError>
#include "tag.h"
#include "image.h"
#include "functions.h"
#include "profile.h"



class Page;
class Image;
class Api;

class PageApi : public QObject
{
	Q_OBJECT

	public:
		enum LoadResult
		{
			Ok,
			Error
		};

		explicit PageApi(Page *parentPage, Site *site, Api *api, QStringList tags = QStringList(), int page = 1, int limit = 25, QStringList postFiltering = QStringList(), bool smart = false, QObject *parent = 0, int pool = 0, int lastPage = 0, int lastPageMinId = 0, int lastPageMaxId = 0);
		~PageApi();
		void			setLastPage(Page *page);
		void			load(bool rateLimit = false);
		void			loadTags();
		QUrl			parseUrl(QString url, int pid = -1, int p = -1, QString t = "", QString pseudo = "", QString password = "");
		QList<Image*>	images();
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
		int				minId();
		int				maxId();
		void			setUrl(QUrl url);
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
		void parseImage(QMap<QString,QString> data, int position);
		void parseNavigation(const QString &source);

	private:
		Page			*m_parentPage;
		Site			*m_site;
		Api				*m_api;
		Profile			*m_profile;
		QStringList		m_search, m_postFiltering, m_errors;
		int				m_imagesPerPage, m_currentSource, m_lastPage, m_lastPageMinId, m_lastPageMaxId, m_imagesCount, m_pagesCount, m_page, m_blim, m_pool;
		bool			m_smart;
		QString			m_format, m_source, m_wiki, m_originalUrl;
		QUrl			m_url, m_urlRegex, m_urlNextPage, m_urlPrevPage;
		QList<Image*>	m_images;
		QList<Tag>		m_tags;
		QNetworkReply	*m_reply, *m_replyTags;
};

#endif // PAGE_API_H
