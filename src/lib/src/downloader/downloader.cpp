#include "downloader/downloader.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QObject>
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


Downloader::Downloader(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sources, int page, int max, int perPage, QString location, QString filename, QString user, QString password, bool blacklist, Blacklist blacklistedTags, bool noDuplicates, int tagsMin, bool loadMoreDetails, Downloader *previous, bool login)
	: m_profile(profile), m_printer(printer), m_lastPage(nullptr), m_tags(std::move(tags)), m_postFiltering(std::move(postFiltering)), m_sites(std::move(sources)), m_page(page), m_max(max), m_perPage(perPage), m_waiting(0), m_ignored(0), m_duplicates(0), m_tagsMin(tagsMin), m_loadMoreDetails(loadMoreDetails), m_location(std::move(location)), m_filename(std::move(filename)), m_user(std::move(user)), m_password(std::move(password)), m_blacklist(blacklist), m_noDuplicates(noDuplicates), m_blacklistedTags(std::move(blacklistedTags)), m_cancelled(false), m_quit(false), m_login(login), m_previous(previous)
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
		returnInt(total);
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
		returnTagList(list);
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

	m_waiting = 0;
	m_cancelled = false;

	for (Site *site : qAsConst(m_sites)) {
		int pages = qCeil(static_cast<qreal>(m_max) / m_perPage);
		if (pages <= 0 || m_perPage <= 0 || m_max <= 0) {
			pages = 1;
		}
		for (int p = 0; p < pages; ++p) {
			m_pagesP.append(QPair<Site*, int>(site, m_page + p));
			m_oPagesP.append(QPair<Site*, int>(site, m_page + p));
			m_waiting++;
		}
	}

	loadNext();
}
Api *getTagApi(Site *site)
{
	for (Api *a : site->getApis()) {
		if (a->canLoadTags()) {
			return a;
		}
	}
	return nullptr;
}
void Downloader::loadNext()
{
	if (m_cancelled) {
		return;
	}

	if (!m_oPagesP.isEmpty()) {
		log(QStringLiteral("Loading tags"));
		QPair<Site*, int> tag = m_oPagesP.takeFirst();
		Site *site = tag.first;

		Api *api = getTagApi(site);
		if (api == nullptr) {
			log(QStringLiteral("No valid API for loading tags for source: %1").arg(site->url()), Logger::Error);
			return;
		}

		auto *tagApi = new TagApi(m_profile, site, api, tag.second, m_perPage, "count", this);
		connect(tagApi, &TagApi::finishedLoading, this, &Downloader::finishedLoadingTags);
		tagApi->load();
		return;
	}

	if (!m_images.isEmpty()) {
		const QSharedPointer<Image> image = m_images.takeFirst();
		log(QString("Loading image '%1'").arg(image->url().toString()));
		auto dwl = new ImageDownloader(m_profile, image, m_filename, m_location, 0, true, false, this);
		if (!m_blacklist) {
			dwl->setBlacklist(&m_blacklistedTags);
		}
		connect(dwl, &ImageDownloader::saved, this, &Downloader::finishedLoadingImage);
		connect(dwl, &ImageDownloader::saved, dwl, &ImageDownloader::deleteLater);
		dwl->save();
		return;
	}
}
void Downloader::finishedLoadingTags(TagApiBase *a, TagApi::LoadResult status)
{
	if (m_cancelled) {
		return;
	}

	if (status == TagApi::LoadResult::Error) {
		log(QStringLiteral("Error loading pure tags"), Logger::Warning);
		return;
	}

	const auto api = dynamic_cast<TagApi*>(a);
	const QList<Tag> tags = api->tags();
	log(QStringLiteral("Received pure tags (%1)").arg(tags.count()));
	api->deleteLater();

	m_results.append(tags);
	if (--m_waiting > 0) {
		loadNext();
		return;
	}

	QMutableListIterator<Tag> i(m_results);
	while (i.hasNext()) {
		if (i.next().count() < m_tagsMin) {
			i.remove();
		}
	}

	if (m_quit) {
		returnTagList(m_results);
	} else {
		emit finishedTags(m_results);
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
	downloadImages(images);
}

void Downloader::downloadImages(const QList<QSharedPointer<Image>> &images)
{
	m_images.clear();
	m_images.append(images);
	m_waiting = images.size();

	loadNext();
}
void Downloader::finishedLoadingImage(const QSharedPointer<Image> &image, const QList<ImageSaveResult> &result)
{
	Q_UNUSED(result);

	if (m_cancelled) {
		return;
	}

	log(QStringLiteral("Received image '%1'").arg(image->url().toString()));

	if (!m_quit) {
		emit finishedImage(image);
	}

	if (--m_waiting > 0) {
		loadNext();
		return;
	}

	if (m_quit) {
		returnString(QStringLiteral("Downloaded images successfully."));
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
		returnImageList(images);
	} else {
		emit finishedImages(images);
	}
}

void Downloader::returnInt(int ret)
{
	m_printer->print(ret);
	emit quit();
}
void Downloader::returnString(const QString &ret)
{
	m_printer->print(ret);
	emit quit();
}
void Downloader::returnTagList(const QList<Tag> &tags)
{
	m_printer->print(tags);
	emit quit();
}
void Downloader::returnImageList(const QList<QSharedPointer<Image>> &ret)
{
	m_printer->print(ret);
	emit quit();
}

void Downloader::setData(const QVariant &data)
{ m_data = data; }
const QVariant &Downloader::getData() const
{ return m_data; }

void Downloader::cancel()
{ m_cancelled = true; }

int Downloader::ignoredCount() const
{ return m_ignored; }
int Downloader::duplicatesCount() const
{ return m_duplicates; }
int Downloader::pagesCount() const
{
	int pages = qCeil(static_cast<qreal>(m_max) / m_perPage);
	if (pages <= 0 || m_perPage <= 0 || m_max <= 0) {
		pages = 1;
	}
	return pages * m_sites.size();
}
int Downloader::imagesMax() const
{ return m_max; }
Page *Downloader::lastPage() const
{ return m_lastPage; }

const QList<Site*> &Downloader::getSites() const
{ return m_sites; }
