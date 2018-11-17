#include "models/page-api.h"
#include <QNetworkReply>
// #include <QtConcurrentRun>
#include <QTimer>
#include <QtMath>
#include "functions.h"
#include "image.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/filtering/post-filter.h"
#include "models/page.h"
#include "models/site.h"
#include "tags/tag.h"


PageApi::PageApi(Page *parentPage, Profile *profile, Site *site, Api *api, QStringList tags, int page, int limit, PostFilter postFiltering, bool smart, QObject *parent, int pool, int lastPage, qulonglong lastPageMinId, qulonglong lastPageMaxId)
	: QObject(parent), m_parentPage(parentPage), m_profile(profile), m_site(site), m_api(api), m_search(std::move(tags)), m_errors(QStringList()), m_postFiltering(std::move(postFiltering)), m_imagesPerPage(limit), m_lastPage(lastPage), m_lastPageMinId(lastPageMinId), m_lastPageMaxId(lastPageMaxId), m_smart(smart), m_reply(nullptr), m_replyTags(nullptr)
{
	m_imagesCount = -1;
	m_maxImagesCount = -1;
	m_pagesCount = -1;
	m_imagesCountSafe = false;
	m_pagesCountSafe = false;

	m_page = page;
	m_pool = pool;
	m_format = m_api->getName();

	m_replyTimer = new QTimer(this);
	m_replyTimer->setSingleShot(true);
	connect(m_replyTimer, &QTimer::timeout, this, &PageApi::loadNow);

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
	QString search = m_search.join(' ');
	m_errors.clear();

	// Gallery searches using either 'gallery:url' or 'gallery:id'
	const bool isGallery = m_search.count() == 1 && search.startsWith("gallery:");
	if (isGallery)
	{ search = search.mid(8); }

	// URL searches
	if (m_search.count() == 1 && !search.isEmpty() && isUrl(search))
	{ url = search; }
	else
	{
		PageUrl ret;
		if (isGallery)
		{ ret = m_api->galleryUrl(search, m_page, m_imagesPerPage, m_site); }
		else
		{ ret = m_api->pageUrl(search, m_page, m_imagesPerPage, m_lastPage, m_lastPageMinId, m_lastPageMaxId, m_site); }

		if (!ret.error.isEmpty())
		{ m_errors.append(ret.error); }
		url = ret.url;
	}

	// Add site information to URL
	url = m_site->fixUrl(url).toString();

	m_originalUrl = QString(url);
	m_url = QString(url);
	m_urlRegex = QUrl(url);
}

void PageApi::setReply(QNetworkReply *reply)
{
	if (m_reply != nullptr)
	{
		if (m_reply->isRunning())
			m_reply->abort();

		m_reply->deleteLater();
	}

	if (m_replyTimer->isActive())
		m_replyTimer->stop();

	m_reply = reply;
}

void PageApi::load(bool rateLimit, bool force)
{
	if (m_loading)
	{
		if (!force)
			return;

		setReply(nullptr);
	}

	if (m_url.isEmpty() && !m_errors.isEmpty())
	{
		for (const QString &err : qAsConst(m_errors))
		{ log(QStringLiteral("[%1][%2] %3").arg(m_site->url(), m_format, err), Logger::Warning); }
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	// Reading reply and resetting vars
	m_images.clear();
	m_tags.clear();
	m_loaded = false;
	m_loading = true;
	m_pageImageCount = 0;
	m_imagesCount = -1;
	m_maxImagesCount = -1;
	m_pagesCount = -1;

	// Load the request with a possible delay
	int ms = m_site->msToRequest(rateLimit ? Site::QueryType::Retry : Site::QueryType::List);
	if (ms > 0)
	{
		if (m_replyTimer->isActive())
			m_replyTimer->stop();

		m_replyTimer->setInterval(ms);
		m_replyTimer->start();
	}
	else
	{ loadNow(); }
}
void PageApi::loadNow()
{
	log(QStringLiteral("[%1][%2] Loading page `%3`").arg(m_site->url(), m_format, m_url.toString().toHtmlEscaped()), Logger::Info);
	setReply(m_site->get(m_url));
	connect(m_reply, &QNetworkReply::finished, this, &PageApi::parse);
}
void PageApi::abort()
{
	if (m_replyTimer->isActive())
		m_replyTimer->stop();
	if (m_reply != nullptr && m_reply->isRunning())
		m_reply->abort();
}

bool PageApi::addImage(const QSharedPointer<Image> &img)
{
	if (img.isNull())
		return false;

	m_pageImageCount++;

	QStringList filters = m_postFiltering.match(img->tokens(m_profile));
	if (!filters.isEmpty())
	{
		img->deleteLater();
		log(QStringLiteral("[%1][%2] Image filtered. Reason: %3.").arg(m_site->url(), m_format, filters.join(", ")), Logger::Info);
		return false;
	}

	m_images.append(img);
	return true;
}

void PageApi::parse()
{
	if (m_reply == nullptr)
		return;

	log(QStringLiteral("[%1][%2] Receiving page `%3`").arg(m_site->url(), m_format, m_reply->url().toString().toHtmlEscaped()), Logger::Info);

	// Check redirection
	QUrl redir = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		QUrl newUrl = m_site->fixUrl(redir.toString(), m_url);
		log(QStringLiteral("[%1][%2] Redirecting page `%3` to `%4`").arg(m_site->url(), m_format, m_url.toString().toHtmlEscaped(), newUrl.toString().toHtmlEscaped()), Logger::Info);

		// HTTP -> HTTPS redirects
		const bool ssl = m_site->setting("ssl", false).toBool();
		if (!ssl && newUrl.path() == m_url.path() && newUrl.scheme() == "https" && m_url.scheme() == "http")
		{
			const bool notThisSite = m_site->setting("ssl_never_correct", false).toBool();
			if (!notThisSite)
			{ emit httpsRedirect(); }
		}

		m_url = newUrl;
		load(false, true);
		return;
	}

	// Detect HTTP 429 usage limit reached
	const int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 429)
	{
		log(QStringLiteral("[%1][%2] Limit reached (429). New try.").arg(m_site->url(), m_format), Logger::Warning);
		load(true, true);
		return;
	}

	// QtConcurrent::run(this, &PageApi::parseActual);
	parseActual();
}

void PageApi::parseActual()
{
	// Try to read the reply
	m_source = m_reply->readAll();
	if (m_source.isEmpty() || m_reply->error() != QNetworkReply::NoError)
	{
		if (m_reply->error() != QNetworkReply::OperationCanceledError)
		{ log(QStringLiteral("[%1][%2] Loading error: %3 (%4)").arg(m_site->url(), m_format, m_reply->errorString()).arg(m_reply->error()), Logger::Error); }
		setReply(nullptr);
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	const int first = m_smart && m_blim > 0 ? ((m_page - 1) * m_imagesPerPage) % m_blim : 0;

	// Parse source
	ParsedPage page;
	if (m_search.count() == 1 && m_search[0].startsWith("gallery:"))
	{ page = m_api->parseGallery(m_parentPage, m_source, first); }
	else
	{ page = m_api->parsePage(m_parentPage, m_source, first); }

	// Handle errors
	if (!page.error.isEmpty())
	{
		m_errors.append(page.error);
		log(QStringLiteral("[%1][%2] %3").arg(m_site->url(), m_format, page.error), Logger::Warning);
		setReply(nullptr);
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	// Fill data from parsing result
	if (page.pageCount >= 0)
	{ setPageCount(page.pageCount, true); }
	if (page.imageCount >= 0)
	{ setImageCount(page.imageCount, true); }
	for (const Tag &tag : qAsConst(page.tags))
	{ m_tags.append(tag); }
	for (const QSharedPointer<Image> &img : qAsConst(page.images))
	{ addImage(img); }
	if (page.urlNextPage.isValid())
	{ m_urlNextPage = page.urlNextPage; }
	if (page.urlPrevPage.isValid())
	{ m_urlPrevPage = page.urlPrevPage; }
	if (!page.wiki.isEmpty())
	{ m_wiki = fixCloudflareEmails(page.wiki); }

	// Complete image count information from tag count information
	if (m_imagesCount < 1 || !m_imagesCountSafe)
	{
		int found = 0;
		int min = -1;
		for (const Tag &tag : qAsConst(m_tags))
		{
			if (m_search.contains(tag.text()))
			{
				found++;
				if (min == -1 || min > tag.count())
				{ min = tag.count(); }
			}
		}
		int searchTagsCount = m_search.count();;
		if (m_search.count() > found)
		{
			const QStringList modifiers = QStringList() << "-" << m_api->modifiers();
			for (const QString &search : qAsConst(m_search))
			{
				for (const QString &modifier : modifiers)
				{
					if (search.startsWith(modifier))
					{
						searchTagsCount--;
						break;
					}
				}
			}
		}
		if (searchTagsCount == found)
		{
			if (m_search.count() == 1)
			{
				const int forcedLimit = m_api->forcedLimit();
				const int perPage = forcedLimit > 0 ? forcedLimit : m_imagesPerPage;
				const int expectedPageCount = qCeil(static_cast<qreal>(min) / perPage);
				setImageCount(min, m_pagesCountSafe && expectedPageCount == m_pagesCount);
			}
			setImageMaxCount(min);
		}
	}

	// Complete missing tag information from images' tags if necessary
	if (m_tags.isEmpty())
	{
		QStringList tagsGot;
		for (const QSharedPointer<Image> &img : qAsConst(m_images))
		{
			for (const Tag &tag : img->tags())
			{
				if (tagsGot.contains(tag.text()))
				{
					const int index = tagsGot.indexOf(tag.text());
					m_tags[index].setCount(m_tags[index].count() + 1);
				}
				else
				{
					m_tags.append(tag);
					tagsGot.append(tag.text());
				}
			}
		}
	}

	// Remove first n images (according to site settings)
	int skip = m_site->setting("ignore/always", 0).toInt();
	if (false && m_isAltPage) // FIXME(Bionus): broken since move to Api class
	{ skip = m_site->setting("ignore/alt", 0).toInt(); }
	if (m_page == 1)
	{ skip = m_site->setting("ignore/1", 0).toInt(); }
	if (m_api->getName() == QLatin1String("Html"))
	{
		if (m_images.size() >= skip)
		{
			for (int i = 0; i < skip; ++i)
			{
				m_images.removeFirst();
				m_pageImageCount--;
			}
		}
		else
		{ log(QStringLiteral("Wanting to skip %1 images but only %2 returned").arg(skip).arg(m_images.size()), Logger::Warning); }
	}

	// Virtual paging
	int firstImage = 0;
	int lastImage = m_smart ? m_imagesPerPage : m_images.size();
	if (false && !m_originalUrl.contains("{page}") && !m_originalUrl.contains("{cpage}") && !m_originalUrl.contains("{pagepart}") && !m_originalUrl.contains("{pid}")) // TODO(Bionus): add real virtual paging
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

	log(QStringLiteral("[%1][%2] Parsed page `%3`: %4 images (%5), %6 tags (%7), %8 total (%9), %10 pages (%11)").arg(m_site->url(), m_format, m_reply->url().toString().toHtmlEscaped()).arg(m_images.count()).arg(m_pageImageCount).arg(page.tags.count()).arg(m_tags.count()).arg(imagesCount(false)).arg(imagesCount(true)).arg(pagesCount(false)).arg(pagesCount(true)), Logger::Info);

	setReply(nullptr);
	m_loaded = true;
	m_loading = false;

	emit finishedLoading(this, LoadResult::Ok);
}

void PageApi::clear()
{
	m_images.clear();
	m_pageImageCount = 0;
}

const QList<QSharedPointer<Image>> &PageApi::images() const { return m_images; }
const QUrl &PageApi::url() const { return m_url; }
const QString &PageApi::source() const { return m_source; }
const QString &PageApi::wiki() const { return m_wiki; }
const QList<Tag> &PageApi::tags() const { return m_tags; }
const QStringList &PageApi::search() const { return m_search; }
const QStringList &PageApi::errors() const { return m_errors; }
const QUrl &PageApi::nextPage() const { return m_urlNextPage; }
const QUrl &PageApi::prevPage() const { return m_urlPrevPage; }
bool PageApi::isLoaded() const { return m_loaded; }

int PageApi::imagesPerPage() const
{ return m_imagesPerPage; }
int PageApi::page() const
{ return m_page; }
int PageApi::pageImageCount() const
{ return m_pageImageCount; }
int PageApi::highLimit() const
{ return m_api->maxLimit(); }

bool PageApi::hasNext() const
{
	int pageCount = pagesCount();
	int maxPages = maxPagesCount();
	if (pageCount <= 0 && maxPages > 0)
		pageCount = maxPages;

	return pageCount > m_page || (pageCount <= 0 && m_pageImageCount > 0);
}

bool PageApi::isImageCountSure() const { return m_imagesCountSafe; }
int PageApi::imagesCount(bool guess) const
{
	if (m_imagesCountSafe)
		return m_imagesCount;

	if (m_pagesCount == 1)
		return m_pageImageCount;

	if (!guess)
		return -1;

	if (m_imagesCount < 0 && m_pagesCount >= 0)
	{
		const int forcedLimit = m_api->forcedLimit();
		const int perPage = forcedLimit > 0 ? forcedLimit : m_imagesPerPage;
		return m_pagesCount * perPage;
	}

	return m_imagesCount;
}
int PageApi::maxImagesCount() const
{ return m_maxImagesCount; }
bool PageApi::isPageCountSure() const { return m_pagesCountSafe; }
int PageApi::pagesCount(bool guess) const
{
	if (m_pagesCountSafe)
		return m_pagesCount;

	if (!guess)
		return -1;

	if (m_pagesCount < 0 && m_imagesCount >= 0)
	{
		const int forcedLimit = m_api->forcedLimit();
		const int perPage = forcedLimit > 0 ? forcedLimit : m_imagesPerPage;
		return qCeil(static_cast<qreal>(m_imagesCount) / perPage);
	}

	return m_pagesCount;
}
int PageApi::maxPagesCount() const
{
	if (m_maxImagesCount < 0)
		return -1;

	const int forcedLimit = m_api->forcedLimit();
	const int perPage = forcedLimit > 0 ? forcedLimit : m_imagesPerPage;
	return qCeil(static_cast<qreal>(m_maxImagesCount) / perPage);
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
		{
			const int forcedLimit = m_api->forcedLimit();
			const int perPage = forcedLimit > 0 ? forcedLimit : m_imagesPerPage;
			setPageCount(qCeil(static_cast<qreal>(count) / perPage), true);
		}
	}
}
void PageApi::setImageMaxCount(int maxCount)
{ m_maxImagesCount = maxCount; }

void PageApi::setPageCount(int count, bool sure)
{
	if (m_pagesCount <= 0 || (!m_pagesCountSafe && sure))
	{
		m_pagesCount = count;
		m_pagesCountSafe = sure;

		if (sure)
		{
			const int forcedLimit = m_api->forcedLimit();
			const int perPage = forcedLimit > 0 ? forcedLimit : m_imagesPerPage;
			setImageCount(count * perPage, false);
		}
	}
}
