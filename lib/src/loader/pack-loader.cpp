#include "loader/pack-loader.h"
#include <QEventLoop>
#include <QtMath>
#include "models/image.h"
#include "models/page.h"
#include "models/site.h"
#include "models/source.h"


PackLoader::PackLoader(Profile *profile, DownloadQueryGroup query, int packSize, QObject *parent)
	: QObject(parent), m_profile(profile), m_site(query.site), m_query(std::move(query)), m_packSize(packSize)
{}

const DownloadQueryGroup &PackLoader::query() const { return m_query; }
int PackLoader::nextPackSize() const { return qMin(m_packSize, m_query.total - m_total); }

bool PackLoader::start()
{
	// Login to the site
	QEventLoop loop;
	QObject::connect(m_site, &Site::loggedIn, &loop, &QEventLoop::quit, Qt::QueuedConnection);
	m_site->login();
	loop.exec();

	// Resume stopped downloads
	int page = m_query.page;
	if (m_query.progressVal > 0) {
		const int pagesToSkip = qFloor(m_query.progressVal / m_query.perpage);
		page += pagesToSkip;
		m_total = pagesToSkip * m_query.perpage;
	}

	// Add the first results page
	m_pendingPages.append(new Page(m_profile, m_site, QList<Site*>() << m_site, m_query.query, page, m_query.perpage, m_query.postFiltering, false, nullptr));

	return true;
}

void PackLoader::abort()
{
	m_abort = true;
}

bool PackLoader::hasNext() const
{
	return (!m_overflow.isEmpty() || !m_pendingPages.isEmpty() || !m_pendingGalleries.isEmpty()) && (m_total < m_query.total || m_query.total < 0);
}

QList<QSharedPointer<Image>> PackLoader::next()
{
	const int maxPages = qMax(1, qCeil(static_cast<qreal>(m_packSize) / m_query.perpage));
	const int already = m_total;

	QList<QSharedPointer<Image>> results;
	int pageCount = 0;

	if (!m_overflow.isEmpty()) {
		while (!m_overflow.isEmpty() && (results.isEmpty() || results.count() < m_packSize || m_packSize < 0) && (already + results.count() != m_query.total || (m_overflowGallery && m_query.galleriesCountAsOne))) {
			results.append(m_overflow.takeFirst());
		}

		if (!m_overflowGallery || !m_query.galleriesCountAsOne) {
			m_total += results.count();
		}

		// If the overflow was the end of a gallery and we finished it, increase the counter
		if (m_overflowGallery && m_overflow.isEmpty() && m_query.galleriesCountAsOne && !m_overflowHasNext) {
			m_total++;
		}
	}

	while (hasNext() && pageCount < maxPages && (results.isEmpty() || results.count() < m_packSize || m_packSize < 0)) {
		if (m_abort) {
			m_abort = false;
			break;
		}

		bool gallery = !m_pendingGalleries.isEmpty();

		// Load next page/gallery
		QEventLoop loop;
		Page *page = gallery ? m_pendingGalleries.takeFirst() : m_pendingPages.takeFirst();
		QObject::connect(page, &Page::finishedLoading, &loop, &QEventLoop::quit);
		QObject::connect(page, &Page::failedLoading, &loop, &QEventLoop::quit);
		page->load(false);
		loop.exec();
		emit finishedPage(page);

		// Add next page to the pending queue
		if (page->hasNext()) {
			Page *next = new Page(m_profile, m_site, QList<Site*>() << m_site, page->query(), page->page() + 1, m_query.perpage, m_query.postFiltering, false, nullptr);
			next->setLastPage(page);
			if (gallery) {
				m_pendingGalleries.prepend(next);
			} else {
				m_pendingPages.append(next);
			}
		}

		// Add results to the data object
		auto itGallery = m_pendingGalleries.begin();
		for (const QSharedPointer<Image> &img : page->images()) {
			// If this result is a gallery, add it to the beginning of the pending galleries
			if (img->isGallery()) {
				SearchQuery q;
				q.gallery = img;
				q.tags = page->search();
				Page *galleryPage = new Page(m_profile, m_site, QList<Site*>() << m_site, q, 1, m_query.perpage, m_query.postFiltering, false, nullptr);
				m_pendingGalleries.insert(itGallery, galleryPage);
				continue;
			}

			// If it's an image, add it to the results
			if (results.count() >= m_packSize) {
				m_overflow.append(img);
				m_overflowGallery = gallery;
				m_overflowHasNext = page->hasNext();
			} else {
				results.append(img);

				if (!gallery || !m_query.galleriesCountAsOne) {
					m_total++;
				}
			}

			// Early return if we reached the image limit
			if (m_total == m_query.total) {
				break;
			}
		}

		// If it's the last page of a gallery, increase the counter if we treated all images
		if (gallery && !page->hasNext() && m_query.galleriesCountAsOne && m_overflow.isEmpty()) {
			m_total++;
		}

		if (!gallery) {
			pageCount++;
		}

		page->deleteLater();
	}

	return results;
}
