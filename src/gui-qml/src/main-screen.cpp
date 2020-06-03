#include "main-screen.h"
#include <QEventLoop>
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"

#define DEFAULT_SITE "danbooru.donmai.us"
#define IMAGES_PER_PAGE 20


MainScreen::MainScreen(Profile *profile, QObject *parent)
	: QObject(parent), m_profile(profile)
{}

void MainScreen::search(const QString &query, int pageNumber)
{
	m_query = query;
	emit queryChanged();

	Site *site = m_profile->getSites().value(DEFAULT_SITE);
	Page *page = new Page(m_profile, site, m_profile->getSites().values(), query.split(' '), pageNumber, IMAGES_PER_PAGE, {}, false, this);

	QEventLoop loop;
	QObject::connect(page, &Page::finishedLoading, &loop, &QEventLoop::quit);
	QObject::connect(page, &Page::failedLoading, &loop, &QEventLoop::quit);
	page->load(false);
	loop.exec();

	QList<QSharedPointer<Image>> results = page->images();

	m_thumbnails.clear();
	m_thumbnails.reserve(results.count());
	for (const QSharedPointer<Image> &img : results) {
		m_thumbnails.append(new ImagePreview(img->url(Image::Size::Thumbnail).toString(), this));
	}

	emit thumbnailsChanged();
}
