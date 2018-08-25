#include "loader/pack-loader.h"
#include <QEventLoop>
#include "models/image.h"
#include "models/page.h"
#include "models/site.h"
#include "models/source.h"


PackLoader::PackLoader(Profile *profile, DownloadQueryGroup query, int packSize)
	: m_profile(profile), m_site(query.site), m_query(std::move(query)), m_packSize(packSize)
{}

bool PackLoader::start()
{
	// Login to the site
	QEventLoop loop;
	QObject::connect(m_site, &Site::loggedIn, &loop, &QEventLoop::quit, Qt::QueuedConnection);
	m_site->login();
	loop.exec();

	// Add the first results page
	m_pendingPages.append(new Page(m_profile, m_site, QList<Site*>() << m_site, m_query.tags.split(' '), m_query.page, m_query.perpage, m_query.postFiltering, false, nullptr));

	return true;
}

bool PackLoader::hasNext() const
{
	return (!m_pendingPages.isEmpty() || !m_pendingGalleries.isEmpty()) && (m_total < m_query.total || m_query.total < 0);
}

QList<QSharedPointer<Downloadable>> PackLoader::next()
{
	QList<QSharedPointer<Downloadable>> results;

	while (hasNext() && (results.count() < m_packSize || m_packSize < 0))
	{
		bool gallery = !m_pendingGalleries.isEmpty();

		// Load next page/gallery
		QEventLoop loop;
		Page *page = gallery ? m_pendingGalleries.takeFirst() : m_pendingPages.takeFirst();
		QObject::connect(page, &Page::finishedLoading, &loop, &QEventLoop::quit);
		QObject::connect(page, &Page::failedLoading, &loop, &QEventLoop::quit);
		page->load(false);
		loop.exec();

		// Add next page to the pending queue
		if (!gallery || page->hasNext())
		{
			Page *next = new Page(m_profile, m_site, QList<Site*>() << m_site, page->search(), page->page() + 1, m_query.perpage, m_query.postFiltering, false, nullptr);
			if (gallery)
				m_pendingGalleries.prepend(next);
			else
				m_pendingPages.append(next);
		}

		// Add results to the data object
		auto itGallery = m_pendingGalleries.begin();
		for (const QSharedPointer<Image> &img : page->images())
		{
			// If this result is a gallery, add it to the beginning of the pending galleries
			if (img->isGallery())
			{
				Page *gallery = new Page(m_profile, m_site, QList<Site*>() << m_site, QStringList() << ("gallery:" + img->md5()), 1, m_query.perpage, m_query.postFiltering, false, nullptr);
				// gallery->addToken("gallery_name", img->name());
				m_pendingGalleries.insert(itGallery, gallery);
				continue;
			}

			// If it's an image, add it to the results
			results.append(img);
			m_total++;
		}

		page->deleteLater();
	}

	return results;
}
