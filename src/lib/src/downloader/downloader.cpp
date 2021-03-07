#include "downloader/downloader.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QObject>
#include <QSettings>
#include <qmath.h>
#include <iostream>
#include <utility>
#include "downloader/download-query-group.h"
#include "downloader/image-downloader.h"
#include "downloader/printers/printer.h"
#include "functions.h"
#include "loader/pack-loader.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "tags/tag.h"
#include "tags/tag-api.h"


void loadMoreDetails(const QList<QSharedPointer<Image>> &images)
{
	int work = images.length();
	QEventLoop loop;
	int requestsLimit = 5;  // simultan requests
	int runningRequests = 0;
	for (auto& image : images) {
		while (runningRequests >= requestsLimit) {
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
		}
		runningRequests++;
		image->loadDetails();
		QObject::connect(image.data(), &Image::finishedLoadingTags, [&](){
			work--;
			runningRequests--;
			if (!work) {
				loop.quit();
			}
		});
	}
	loop.exec();
}


Downloader::Downloader(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sources, int page, int max, int perPage, QString location, QString filename, QString user, QString password, bool blacklist, Blacklist blacklistedTags, bool noDuplicates, int tagsMin, bool loadMoreDetails, bool login)
	: m_profile(profile), m_printer(printer), m_lastPage(nullptr), m_tags(std::move(tags)), m_postFiltering(std::move(postFiltering)), m_sites(std::move(sources)), m_page(page), m_max(max), m_perPage(perPage), m_waiting(0), m_ignored(0), m_duplicates(0), m_tagsMin(tagsMin), m_loadMoreDetails(loadMoreDetails), m_location(std::move(location)), m_filename(std::move(filename)), m_user(std::move(user)), m_password(std::move(password)), m_blacklist(blacklist), m_noDuplicates(noDuplicates), m_blacklistedTags(std::move(blacklistedTags)), m_quit(false), m_login(login)
{}

void Downloader::setQuit(bool quit)
{
	m_quit = quit;
}


QList<Page*> Downloader::getAllPagesTags()
{
	QList<Page*> pages;

	for (auto *site : m_sites) {
		auto *page = new Page(m_profile, site, m_sites, m_tags, m_page, m_perPage, m_postFiltering, true, this);

		QEventLoop loop;
		QObject::connect(page, &Page::finishedLoadingTags, &loop, &QEventLoop::quit, Qt::QueuedConnection);
		page->loadTags();
		loop.exec();

		pages.append(page);
	}

	return pages;
}

void Downloader::getPageCount()
{
	if (m_sites.empty()) {
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	const auto pages = getAllPagesTags();

	int total = 0;
	for (Page *p : pages) {
		total += p->imagesCount();
	}

	qDeleteAll(pages);

	if (m_quit) {
		m_printer->print(total);
		emit quit();
	} else {
		emit finishedPageCount(total);
	}
}

void Downloader::getPageTags()
{
	if (m_sites.empty()) {
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	const auto pages = getAllPagesTags();

	QList<Tag> list;
	for (auto p : pages) {
		const QList<Tag> &pageTags = p->tags();
		for (const Tag &tag : pageTags) {
			bool found = false;
			for (auto &t : list) {
				if (t.text() == tag.text()) {
					t.setCount(t.count() + tag.count());
					found = true;
				}
			}
			if (!found) {
				list.append(tag);
			}
		}
	}

	qDeleteAll(pages);

	QMutableListIterator<Tag> i(list);
	while (i.hasNext()) {
		if (i.next().count() < m_tagsMin) {
			i.remove();
		}
	}

	if (m_quit) {
		m_printer->print(list);
		emit quit();
	} else {
		emit finishedTags(list);
	}
}

void Downloader::getTags()
{
	if (m_sites.empty()) {
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	QList<Tag> results;

	for (Site *site : qAsConst(m_sites)) {
		Api *api = site->tagsApi();
		if (api == nullptr) {
			log(QStringLiteral("No valid API for loading tags for source: %1").arg(site->url()), Logger::Error);
			return;
		}

		int pages = qCeil(static_cast<qreal>(m_max) / m_perPage);
		if (pages <= 0 || m_perPage <= 0 || m_max <= 0) {
			pages = 1;
		}

		for (int p = 0; p < pages; ++p) {
			auto *tagApi = new TagApi(m_profile, site, api, m_page + p, m_perPage, "count", this);

			QEventLoop loop;
			QObject::connect(tagApi, &TagApi::finishedLoading, &loop, &QEventLoop::quit, Qt::QueuedConnection);
			tagApi->load();
			loop.exec();

			const QList<Tag> tags = tagApi->tags();
			log(QStringLiteral("Received pure tags (%1)").arg(tags.count()));
			tagApi->deleteLater();

			results.append(tags);
		}
	}

	QMutableListIterator<Tag> i(results);
	while (i.hasNext()) {
		if (i.next().count() < m_tagsMin) {
			i.remove();
		}
	}

	if (m_quit) {
		m_printer->print(results);
		emit quit();
	} else {
		emit finishedTags(results);
	}
}

QList<QSharedPointer<Image>> Downloader::getAllImages()
{
	const bool usePacking = m_profile->getSettings()->value("packing_enable", true).toBool();
	const int imagesPerPack = m_profile->getSettings()->value("packing_size", 1000).toInt();
	const int packSize = usePacking ? imagesPerPack : -1;

	QSet<QString> md5s;
	QList<QSharedPointer<Image>> images;

	for (auto *site : m_sites) {
		DownloadQueryGroup query(m_tags, m_page, m_perPage, m_max, m_postFiltering, m_blacklist, site, m_filename, m_location);

		PackLoader loader(m_profile, query, packSize, nullptr);
		loader.start(m_login);
		while (loader.hasNext()) {
			const auto next = loader.next();
			for (const auto &img : next) {
				if (m_noDuplicates) {
					if (md5s.contains(img->md5())) {
						continue;
					}
					md5s.insert(img->md5());
				}
				images.append(img);
			}
		}
	}

	return images;
}

void Downloader::getImages()
{
	if (m_sites.empty()) {
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	const auto images = getAllImages();

	for (const auto &image : images) {
		ImageDownloader dwl(m_profile, image, m_filename, m_location, 0, true, false, this);
		if (!m_blacklist) {
			dwl.setBlacklist(&m_blacklistedTags);
		}

		QEventLoop loop;
		QObject::connect(&dwl, &ImageDownloader::saved, &loop, &QEventLoop::quit, Qt::QueuedConnection);
		dwl.save();
		loop.exec();

		if (!m_quit) {
			emit finishedImage(image);
		}
	}

	if (m_quit) {
		m_printer->print(QStringLiteral("Downloaded images successfully."));
		emit quit();
	}
}

void Downloader::getUrls()
{
	if (m_sites.empty()) {
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	const auto images = getAllImages();

	if (m_loadMoreDetails) {
		loadMoreDetails(images);
	}

	if (m_quit) {
		m_printer->print(images);
		emit quit();
	} else {
		emit finishedImages(images);
	}
}
