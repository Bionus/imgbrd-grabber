#include "models/page.h"
#include <QUrl>
#include <utility>
#include "analytics.h"
#include "functions.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/search-query/search-query.h"
#include "models/site.h"


Page::Page(Profile *profile, Site *site, const QList<Site*> &sites, SearchQuery query, int page, int limit, const QStringList &postFiltering, bool smart, QObject *parent, int pool, int lastPage, qulonglong lastPageMinId, qulonglong lastPageMaxId)
	: QObject(parent), m_site(site), m_regexApi(-1), m_query(std::move(query)), m_errors(QStringList()), m_imagesPerPage(limit), m_smart(smart)
{
	m_website = m_site->url();
	m_imagesCount = -1;
	m_pagesCount = -1;

	if (!m_query.tags.isEmpty()) {
		// Replace shortcuts to increase compatibility
		QString text = " " + m_query.tags.join(' ') + " ";
		text.replace(" rating:s ", " rating:safe ", Qt::CaseInsensitive)
			.replace(" rating:q ", " rating:questionable ", Qt::CaseInsensitive)
			.replace(" rating:e ", " rating:explicit ", Qt::CaseInsensitive)
			.replace(" -rating:s ", " -rating:safe ", Qt::CaseInsensitive)
			.replace(" -rating:q ", " -rating:questionable ", Qt::CaseInsensitive)
			.replace(" -rating:e ", " -rating:explicit ", Qt::CaseInsensitive);
		QStringList tags = text.split(" ", QString::SkipEmptyParts);

		// Get the list of all enabled modifiers
		QStringList modifiers = QStringList();
		for (Site *ste : sites) {
			modifiers.append(ste->getApis().first()->modifiers());
		}
		const QStringList mods = m_site->getApis().first()->modifiers();
		for (const QString &mod : mods) {
			modifiers.removeAll(mod);
		}

		// Remove modifiers from tags
		for (const QString &mod : modifiers) {
			tags.removeAll(mod);
		}
		m_search = tags;

		m_query.tags = m_search;
	}

	// Generate pages
	PostFilter postFilter(postFiltering);
	m_siteApis = m_site->getLoggedInApis();
	m_pageApis.reserve(m_siteApis.count());
	for (Api *api : qAsConst(m_siteApis)) {
		auto *pageApi = new PageApi(this, profile, m_site, api, m_query, page, limit, postFilter, smart, parent, pool, lastPage, lastPageMinId, lastPageMaxId);
		if (m_pageApis.count() == 0) {
			connect(pageApi, &PageApi::httpsRedirect, this, &Page::httpsRedirectSlot);
		}
		m_pageApis.append(pageApi);
		if (api->getName() == QLatin1String("Html") && m_regexApi < 0) {
			m_regexApi = m_pageApis.count() - 1;
		}
	}
	m_currentApi = -1;

	// Set values
	m_page = page;
	m_pool = pool;
	fallback(false);
}
Page::~Page()
{
	qDeleteAll(m_pageApis);
}

void Page::fallback(bool loadIfPossible)
{
	m_errors.clear();

	if (m_currentApi >= m_siteApis.count() - 1) {
		log(QStringLiteral("[%1] No valid source of the site returned result.").arg(m_site->url()), Logger::Warning);
		m_errors.append(tr("No valid source of the site returned result."));
		emit failedLoading(this);
		return;
	}

	m_currentApi++;
	if (m_currentApi > 0) {
		log(QStringLiteral("[%1] Loading using %2 failed. Retry using %3.").arg(m_site->url(), m_siteApis[m_currentApi - 1]->getName(), m_siteApis[m_currentApi]->getName()), Logger::Warning);
	}

	if (loadIfPossible) {
		load();
	}
}

void Page::setLastPage(Page *page)
{
	for (PageApi *api : qAsConst(m_pageApis)) {
		api->setLastPage(page);
	}

	m_currentApi--;
	fallback(false);
}

void Page::load(bool rateLimit)
{
	connect(m_pageApis[m_currentApi], &PageApi::finishedLoading, this, &Page::loadFinished);
	m_pageApis[m_currentApi]->load(rateLimit);
}
void Page::loadFinished(PageApi *api, PageApi::LoadResult status)
{
	if (api != m_pageApis[m_currentApi]) {
		return;
	}

	QString eventLabel = QStringLiteral("%1 (%2)").arg(m_site->url(), m_siteApis[m_currentApi]->getName());
	if (status == PageApi::LoadResult::Ok) {
		Analytics::getInstance().sendEvent("Page load", "Success", eventLabel);
		emit finishedLoading(this);
	} else {
		Analytics::getInstance().sendEvent("Page load", "Error", eventLabel);
		if (!api->errors().isEmpty()) {
			m_errors.append(api->errors());
		}
		fallback();
	}
}
void Page::abort()
{
	m_pageApis[m_currentApi]->abort();
}

void Page::loadTags()
{
	if (m_regexApi < 0) {
		return;
	}

	connect(m_pageApis[m_regexApi], &PageApi::finishedLoading, this, &Page::loadTagsFinished);
	m_pageApis[m_regexApi]->load();
}
void Page::loadTagsFinished(PageApi *api, PageApi::LoadResult status)
{
	Q_UNUSED(status);

	if (m_regexApi < 0 || api != m_pageApis[m_regexApi]) {
		return;
	}

	emit finishedLoadingTags(this);
}
void Page::abortTags()
{
	if (m_regexApi < 0) {
		return;
	}

	m_pageApis[m_regexApi]->abort();
}

void Page::httpsRedirectSlot()
{ emit httpsRedirect(this); }


void Page::clear()
{
	for (PageApi *pageApi : qAsConst(m_pageApis)) {
		pageApi->clear();
	}
}

Site *Page::site() const { return m_site; }
const QString &Page::website() const { return m_website; }
const QString &Page::wiki() const { return m_pageApis[m_regexApi < 0 ? m_currentApi : m_regexApi]->wiki(); }
const SearchQuery &Page::query() const { return m_query; }
const QStringList &Page::search() const { return m_search; }
const QStringList &Page::errors() const { return m_errors; }
int Page::imagesPerPage() const { return m_imagesPerPage; }
int Page::page() const { return m_page; }
int Page::pageImageCount() const { return m_pageApis[m_currentApi]->pageImageCount(); }
const QList<QSharedPointer<Image>> &Page::images() const { return m_pageApis[m_currentApi]->images(); }
const QUrl &Page::url() const { return m_pageApis[m_currentApi]->url(); }
const QUrl &Page::friendlyUrl() const { return m_pageApis[m_regexApi < 0 ? m_currentApi : m_regexApi]->url(); }
const QList<Tag> &Page::tags() const { return m_pageApis[m_regexApi < 0 ? m_currentApi : m_regexApi]->tags(); }
const QUrl &Page::nextPage() const { return m_pageApis[m_currentApi]->nextPage(); }
const QUrl &Page::prevPage() const { return m_pageApis[m_currentApi]->prevPage(); }
int Page::highLimit() const { return m_pageApis[m_currentApi]->highLimit(); }
bool Page::hasNext() const { return m_pageApis[m_currentApi]->hasNext(); }
bool Page::isLoaded() const { return m_pageApis[m_currentApi]->isLoaded(); }

bool Page::hasSource() const
{
	for (auto pageApi : qAsConst(m_pageApis)) {
		if (!pageApi->source().isEmpty()) {
			return true;
		}
	}
	return false;
}

int Page::imagesCount(bool guess) const
{
	if (m_regexApi >= 0 && !m_pageApis[m_currentApi]->isImageCountSure()) {
		const int count = m_pageApis[m_regexApi]->imagesCount(guess);
		if (count >= 0) {
			return count;
		}
	}
	return m_pageApis[m_currentApi]->imagesCount(guess);
}
int Page::maxImagesCount() const
{
	if (m_regexApi >= 0 && !m_pageApis[m_currentApi]->isImageCountSure()) {
		const int count = m_pageApis[m_regexApi]->maxImagesCount();
		if (count >= 0) {
			return count;
		}
	}
	return m_pageApis[m_currentApi]->maxImagesCount();
}
int Page::pagesCount(bool guess) const
{
	if (m_regexApi >= 0 && !m_pageApis[m_currentApi]->isPageCountSure()) {
		const int count = m_pageApis[m_regexApi]->pagesCount(guess);
		if (count >= 0) {
			return count;
		}
	}
	return m_pageApis[m_currentApi]->pagesCount(guess);
}
int Page::maxPagesCount() const
{
	if (m_regexApi >= 0 && !m_pageApis[m_currentApi]->isPageCountSure()) {
		const int count = m_pageApis[m_regexApi]->maxPagesCount();
		if (count >= 0) {
			return count;
		}
	}
	return m_pageApis[m_currentApi]->maxPagesCount();
}

qulonglong Page::maxId() const
{
	return m_pageApis[m_currentApi]->maxId();
}
qulonglong Page::minId() const
{
	return m_pageApis[m_currentApi]->minId();
}
