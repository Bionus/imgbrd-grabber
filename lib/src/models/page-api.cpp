#include "models/page-api.h"
#include <QDomDocument>
#include <QRegularExpression>
#include <QtMath>
#include "functions.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/image.h"
#include "models/page.h"
#include "models/post-filter.h"
#include "models/site.h"
#include "vendor/json.h"


PageApi::PageApi(Page *parentPage, Profile *profile, Site *site, Api *api, const QStringList &tags, int page, int limit, const QStringList &postFiltering, bool smart, QObject *parent, int pool, int lastPage, qulonglong lastPageMinId, qulonglong lastPageMaxId)
	: QObject(parent), m_parentPage(parentPage), m_profile(profile), m_site(site), m_api(api), m_search(tags), m_postFiltering(postFiltering), m_errors(QStringList()), m_imagesPerPage(limit), m_lastPage(lastPage), m_lastPageMinId(lastPageMinId), m_lastPageMaxId(lastPageMaxId), m_smart(smart), m_reply(nullptr), m_replyTags(nullptr)
{
	m_imagesCount = -1;
	m_pagesCount = -1;
	m_imagesCountSafe = false;
	m_pagesCountSafe = false;

	m_search = tags;
	m_page = page;
	m_pool = pool;
	m_format = m_api->getName();

	updateUrls();
}

void PageApi::setLastPage(Page *page)
{
	m_lastPage = page->page();
	m_lastPageMaxId = page->maxId();
	m_lastPageMinId = page->minId();

	if (!page->nextPage().isEmpty())
	{ m_url = page->nextPage(); }
	else
	{ /*fallback(false);*/ }

	updateUrls();
}

void PageApi::updateUrls()
{
	QString url;
	QString search = m_search.join(" ");

	// URL searches
	if (m_search.count() == 1 && !search.isEmpty() && isUrl(search))
	{ url = search; }
	else
	{ url = m_api->pageUrl(search, m_page, m_imagesPerPage, m_lastPage, m_lastPageMinId, m_lastPageMaxId, m_site); }

	// Add site information to URL
	url = m_site->fixLoginUrl(url, m_api->value("Urls/Login"));
	url = m_site->fixUrl(url).toString();

	m_originalUrl = QString(url);
	m_url = QString(url);
	m_urlRegex = QUrl(url);
}

void PageApi::load(bool rateLimit)
{
	if (m_reply != nullptr)
		return;

	// Reading reply and resetting vars
	m_images.clear();
	m_tags.clear();
	m_loaded = false;
	m_pageImageCount = 0;
	/*m_imagesCount = -1;
	m_pagesCount = -1;*/

	m_site->getAsync(rateLimit ? Site::QueryType::Retry : Site::QueryType::List, m_url, [this](QNetworkReply *reply) {
		log(QString("[%1][%2] Loading page <a href=\"%3\">%3</a>").arg(m_site->url(), m_format, m_url.toString().toHtmlEscaped()), Logger::Info);
		m_reply = reply;
		connect(m_reply, SIGNAL(finished()), this, SLOT(parse()));
	});
}
void PageApi::abort()
{
	if (m_reply != nullptr && m_reply->isRunning())
		m_reply->abort();
}

bool PageApi::addImage(QSharedPointer<Image> img)
{
	if (img.isNull())
		return false;

	m_pageImageCount++;

	QStringList filters = PostFilter::filter(img->tokens(m_profile), m_postFiltering);
	if (!filters.isEmpty())
	{
		img->deleteLater();
		log(QString("[%1][%2] Image filtered. Reason: %3.").arg(m_site->url(), m_format, filters.join(", ")), Logger::Info);
		return false;
	}

	m_images.append(img);
	return true;
}

void PageApi::parse()
{
	log(QString("[%1][%2] Receiving page <a href=\"%3\">%3</a>").arg(m_site->url(), m_format, m_reply->url().toString().toHtmlEscaped()), Logger::Info);

	// Check redirection
	QUrl redir = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		QUrl newUrl = m_site->fixUrl(redir.toString(), m_url);
		log(QString("[%1][%2] Redirecting page <a href=\"%3\">%3</a> to <a href=\"%4\">%4</a>").arg(m_site->url(), m_format, m_url.toString().toHtmlEscaped(), newUrl.toString().toHtmlEscaped()), Logger::Info);
		m_url = newUrl;
		load();
		return;
	}

	// Detect HTTP 429 usage limit reached
	int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 429)
	{
		log(QString("[%1][%2] Limit reached (429). New try.").arg(m_site->url(), m_format), Logger::Warning);
		load(true);
		return;
	}

	// Try to read the reply
	m_source = m_reply->readAll();
	if (m_source.isEmpty())
	{
		if (m_reply->error() != QNetworkReply::OperationCanceledError)
		{ log(QString("[%1][%2] Loading error: %3 (%4)").arg(m_site->url(), m_format, m_reply->errorString()).arg(m_reply->error())); }
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	int first = m_smart && m_blim > 0 ? ((m_page - 1) * m_imagesPerPage) % m_blim : 0;

	// Parse source
	ParsedPage page = m_api->parsePage(m_parentPage, m_source, first);
	if (!page.error.isEmpty())
	{
		log(QString("[%1][%2] %3").arg(m_site->url(), m_format, page.error), Logger::Warning);
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	// Fill data from parsing result
	if (page.imageCount >= 0)
	{ setImageCount(page.imageCount, true); }
	if (page.pageCount >= 0)
	{ setPageCount(page.pageCount, true); }
	for (const Tag &tag : page.tags)
	{ m_tags.append(tag); }
	for (const QSharedPointer<Image> &img : page.images)
	{ addImage(img); }
	if (page.urlNextPage.isValid())
	{ m_urlNextPage = page.urlNextPage; }
	if (page.urlPrevPage.isValid())
	{ m_urlPrevPage = page.urlPrevPage; }
	if (!page.wiki.isEmpty())
	{ m_wiki = page.wiki; }

	// Try to get navigation info on HTML pages
	if (m_format == "Html")
	{ parseNavigation(m_source); }

	// Complete missing tag information from images' tag if necessary
	if (m_tags.isEmpty())
	{
		QStringList tagsGot;
		for (int i = 0; i < m_images.count(); i++)
		{
			QList<Tag> tags = m_images.at(i)->tags();
			for (int t = 0; t < tags.count(); t++)
			{
				if (tagsGot.contains(tags[t].text()))
				{ m_tags[tagsGot.indexOf(tags[t].text())].setCount(m_tags[tagsGot.indexOf(tags[t].text())].count()+1); }
				else
				{
					m_tags.append(tags[t]);
					tagsGot.append(tags[t].text());
				}
			}
		}
	}

	// Remove first n images (according to site settings)
	int skip = m_site->setting("ignore/always", 0).toInt();
	if (m_isAltPage) // FIXME(Bionus): broken since move to Api class
		skip = m_site->setting("ignore/alt", 0).toInt();
	if (m_page == m_site->value("FirstPage").toInt())
		skip = m_site->setting("ignore/1", 0).toInt();
	if (m_images.size() > m_imagesPerPage && m_images.size() > skip)
		for (int i = 0; i < skip; ++i)
			m_images.removeFirst();

	// Virtual paging
	int firstImage = 0;
	int lastImage = m_smart ? m_imagesPerPage : m_images.size();
	if (!m_originalUrl.contains("{page}") && !m_originalUrl.contains("{cpage}") && !m_originalUrl.contains("{pagepart}") && !m_originalUrl.contains("{pid}"))
	{
		firstImage = m_imagesPerPage * (m_page - 1);
		lastImage = m_imagesPerPage;
	}
	while (firstImage > 0 && !m_images.isEmpty())
	{
		m_images.removeFirst();
		firstImage--;
	}
	while (m_images.size() > lastImage)
	{ m_images.removeLast(); }

	log(QString("[%1][%2] Parsed page <a href=\"%3\">%3</a>: %4 images, %5 total (%6), %7 pages (%8)").arg(m_site->url(), m_format, m_reply->url().toString().toHtmlEscaped()).arg(m_images.count()).arg(imagesCount(false)).arg(imagesCount(true)).arg(pagesCount(false)).arg(pagesCount(true)), Logger::Info);

	m_reply->deleteLater();
	m_reply = nullptr;
	m_loaded = true;

	QString t = m_search.join(" ");
	if (m_site->contains("DefaultTag") && t.isEmpty())
	{ t = m_site->value("DefaultTag"); }
	if (!m_search.isEmpty() && !m_api->value("Urls/" + QString(t.isEmpty() && !m_api->contains("Urls/Home") ? "Home" : "Tags")).contains("{tags}"))
	{ m_errors.append(tr("Tag search is impossible with the chosen source (%1).").arg(m_format)); }

	emit finishedLoading(this, LoadResult::Ok);
}

void PageApi::parseNavigation(const QString &source)
{
	// Last page
	if (m_site->contains("LastPage") && m_pagesCount < 1)
	{ setPageCount(m_site->value("LastPage").toInt(), true); }
	if (m_site->contains("Regex/LastPage") && m_pagesCount < 1)
	{
		QRegularExpression rxlast(m_site->value("Regex/LastPage"));
		auto match = rxlast.match(source);
		int cnt = match.hasMatch() ? match.captured(1).remove(",").toInt() : 0;
		if (cnt > 0)
		{
			int pagesCount = cnt;
			if (m_originalUrl.contains("{pid}") || (m_api->contains("Urls/PagePart") && m_api->value("Urls/PagePart").contains("{pid}")))
			{
				int ppid = m_api->contains("Urls/Limit") ? m_api->value("Urls/Limit").toInt() : m_imagesPerPage;
				pagesCount = qFloor(static_cast<float>(pagesCount) / static_cast<float>(ppid)) + 1;
			}
			setPageCount(pagesCount, true);
		}
	}

	// Count images
	if (m_imagesCount < 1)
	{
		for (const Tag &tag : m_tags)
		{
			if (tag.text() == m_search.join(" "))
			{ setImageCount(tag.count(), false); }
		}
	}
}

void PageApi::clear()
{
	m_images.clear();
	m_pageImageCount = 0;
}

QList<QSharedPointer<Image>>	PageApi::images() const	{ return m_images;		}
QUrl			PageApi::url() const		{ return m_url;			}
QString			PageApi::source() const		{ return m_source;		}
QString			PageApi::wiki() const		{ return m_wiki;		}
QList<Tag>		PageApi::tags() const		{ return m_tags;		}
QStringList		PageApi::search() const		{ return m_search;		}
QStringList		PageApi::errors() const		{ return m_errors;		}
QUrl			PageApi::nextPage() const	{ return m_urlNextPage;	}
QUrl			PageApi::prevPage() const	{ return m_urlPrevPage;	}
bool			PageApi::isLoaded() const	{ return m_loaded;		}

int PageApi::imagesPerPage() const
{ return m_imagesPerPage;	}
int PageApi::page() const
{ return m_page;			}
int PageApi::pageImageCount() const
{ return m_pageImageCount;	}
int PageApi::highLimit() const
{ return m_api->maxLimit(); }

bool PageApi::isImageCountSure() const { return m_imagesCountSafe; }
int PageApi::imagesCount(bool guess) const
{
	int perPage = m_api->contains("Urls/Limit") && !m_api->contains("Urls/MaxLimit") ? m_api->value("Urls/Limit").toInt() : m_imagesPerPage;

	if (m_imagesCount < 0 && guess && m_pagesCount >= 0)
		return m_pagesCount * perPage;

	return m_imagesCount;
}
bool PageApi::isPageCountSure() const { return m_pagesCountSafe; }
int PageApi::pagesCount(bool guess) const
{
	int perPage = m_api->contains("Urls/Limit") && !m_api->contains("Urls/MaxLimit") ? m_api->value("Urls/Limit").toInt() : m_imagesPerPage;

	if (m_pagesCount < 0 && guess && m_imagesCount >= 0)
		return qCeil(static_cast<float>(m_imagesCount) / perPage);

	return m_pagesCount;
}

qulonglong PageApi::maxId() const
{
	qulonglong maxId = 0;
	for (const QSharedPointer<Image> &img : m_images)
		if (img->id() > maxId || maxId == 0)
			maxId = img->id();
	return maxId;
}
qulonglong PageApi::minId() const
{
	qulonglong minId = 0;
	for (const QSharedPointer<Image> &img : m_images)
		if (img->id() < minId || minId == 0)
			minId = img->id();
	return minId;
}

void PageApi::setImageCount(int count, bool sure)
{
	if (m_imagesCount <= 0 || (!m_imagesCountSafe && sure))
	{
		m_imagesCount = count;
		m_imagesCountSafe = sure;

		if (sure)
		{ setPageCount(qCeil(static_cast<float>(count) / m_imagesPerPage), true); }
	}
}

void PageApi::setPageCount(int count, bool sure)
{
	if (m_pagesCount <= 0 || (!m_pagesCountSafe && sure))
	{
		m_pagesCount = count;
		m_pagesCountSafe = sure;

		if (sure)
		{ setImageCount(count * m_imagesPerPage, false); }
	}
}
