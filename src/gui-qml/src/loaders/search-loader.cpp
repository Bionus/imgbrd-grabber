#include "search-loader.h"
#include <QEventLoop>
#include <QSettings>
#include <QSharedPointer>
#include <utility>
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/qml-image.h"
#include "models/site.h"


SearchLoader::SearchLoader(QObject *parent)
	: Loader(parent), m_page(1), m_perPage(20), m_profile(nullptr), m_hasPrev(false), m_hasNext(false)
{}


void SearchLoader::search(SearchQuery query)
{
	setStatus(Status::Loading);
	setError("");

	Site *site = m_profile->getSites().value(m_site);
	site->setAutoLogin(false);

	m_profile->getHistory()->addQuery(query, {site});

	QEventLoop loop;
	QObject::connect(site, &Site::loggedIn, &loop, &QEventLoop::quit, Qt::QueuedConnection);
	site->login();
	loop.exec();

	Page *page = new Page(m_profile, site, m_profile->getSites().values(), std::move(query), m_page, m_perPage, m_postFilter.split(' '), false, this);
	connect(page, &Page::finishedLoading, this, &SearchLoader::searchFinished);
	connect(page, &Page::failedLoading, this, &SearchLoader::searchFinished);
	page->load(false);
}

void SearchLoader::searchFinished(Page *page)
{
	if (!page->errors().isEmpty()) {
		setError(page->errors().join('\n'));
		return;
	}

	const QList<QSharedPointer<Image>> &results = page->images();
	const bool hideBlacklisted = m_profile->getSettings()->value("hideblacklisted", false).toBool();

	m_results.clear();
	m_results.reserve(results.count());
	for (const QSharedPointer<Image> &img : results) {
		if (hideBlacklisted && !m_profile->getBlacklist().match(img->tokens(m_profile)).isEmpty()) {
			continue;
		}
		m_results.append(new QmlImage(img, m_profile, this));
	}

	m_hasPrev = page->page() > 1;
	emit hasPrevChanged();

	int pageCount = page->pagesCount();
	int maxPages = page->maxPagesCount();
	if (pageCount <= 0 && maxPages > 0) {
		pageCount = maxPages;
	}
	m_hasNext = pageCount > page->page() || page->imagesCount() == -1 || page->pagesCount() == -1 || (page->imagesCount() == 0 && page->pageImageCount() > 0);
	emit hasNextChanged();

	m_pageCount = page->pagesCount() > 0
		? (page->pagesCount(false) == -1 ? "~" : QString()) + QString::number(page->pagesCount())
		: (page->maxPagesCount() == -1 ? "?" : tr("max %1").arg(page->maxPagesCount()));
	emit pageCountChanged();

	m_imageCount = page->imagesCount() > 0
		? (page->imagesCount(false) == -1 ? "~" : QString()) + QString::number(page->imagesCount())
		: (page->maxImagesCount() == -1 ? "?" : tr("max %1").arg(page->maxImagesCount()));
	emit imageCountChanged();

 	m_profile->addAutoComplete(page->tags());

	emit resultsChanged();
	setStatus(Status::Ready);
}
