#include "main-screen.h"
#include <QDir>
#include <QEventLoop>
#include <QSettings>
#include <QSslSocket>
#include <QStandardPaths>
#include "downloader/image-downloader.h"
#include "functions.h"
#include "logger.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "tags/tag-stylist.h"
#include "utils/logging.h"

#define IMAGES_PER_PAGE 20


MainScreen::MainScreen(Profile *profile, QObject *parent)
	: QObject(parent), m_profile(profile)
{
	connect(&Logger::getInstance(), &Logger::newLog, this, &MainScreen::newLog);
	logSystemInformation(m_profile);

	for (Site *site : m_profile->getSites().values()) {
		m_sites.append(site->url());
	}
	emit sitesChanged();
}

void MainScreen::search(const QString &siteUrl, const QString &query, int pageNumber)
{
	m_query = query;
	emit queryChanged();

	Site *site = m_profile->getSites().value(siteUrl);
	Page *page = new Page(m_profile, site, m_profile->getSites().values(), query.split(' '), pageNumber, IMAGES_PER_PAGE, {}, false, this);

	QEventLoop loop;
	QObject::connect(page, &Page::finishedLoading, &loop, &QEventLoop::quit);
	QObject::connect(page, &Page::failedLoading, &loop, &QEventLoop::quit);
	page->load(false);
	loop.exec();

	QList<QSharedPointer<Image>> results = page->images();

	m_results.clear();
	m_results.reserve(results.count());
	for (const QSharedPointer<Image> &img : results) {
		m_results.append(new ImagePreview(
			img->url(Image::Size::Thumbnail).toString(),
			img->url(Image::Size::Sample).toString(),
			img->url(Image::Size::Full).toString(),
			TagStylist(m_profile).stylished(img->tags(), true, false, "type"),
			img,
			this
		));
	}

	emit resultsChanged();
}

void MainScreen::newLog(const QString &message)
{
	if (!m_log.isEmpty()) {
		m_log += "<br/>";
	}
	m_log += logToHtml(message);

	emit logChanged();
}

void MainScreen::downloadImage(const QSharedPointer<Image> &image)
{
	const QSettings *settings = m_profile->getSettings();
	const QString filename = settings->value("Save/filename", "%md5%.%ext%").toString();
	const QString path = QDir::toNativeSeparators(settings->value("Save/path", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).toString());

	if (filename.isEmpty() || path.isEmpty()) {
		LOG("Empty save filename or directory", Logger::Error);
		return;
	}

	auto downloader = new ImageDownloader(m_profile, image, filename, path, 1, true, true, this, false);
	connect(downloader, &ImageDownloader::saved, downloader, &ImageDownloader::deleteLater);
	downloader->save();
}
