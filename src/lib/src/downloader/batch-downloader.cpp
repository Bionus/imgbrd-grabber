#include "batch-downloader.h"
#include <QCoreApplication>
#include <QSettings>
#include <QTimer>
#include "commands/commands.h"
#include "downloader/download-query-group.h"
#include "downloader/download-query-image.h"
#include "downloader/image-downloader.h"
#include "loader/pack-loader.h"
#include "models/profile.h"
#include "models/site.h"


BatchDownloader::BatchDownloader(DownloadQuery *query, Profile *profile, QObject *parent)
	: QObject(parent), m_query(query), m_profile(profile), m_settings(profile->getSettings()), m_step(BatchDownloadStep::NotStarted)
{}


void BatchDownloader::setCurrentStep(BatchDownloadStep step)
{
	emit stepChanged(step);
	m_step = step;
}

BatchDownloader::BatchDownloadStep BatchDownloader::currentStep() const
{
	return m_step;
}

int BatchDownloader::totalCount() const
{
	return m_totalCount;
}

int BatchDownloader::downloadedCount() const
{
	return m_counterSum;
}

int BatchDownloader::downloadedCount(Counter counter) const
{
	return m_counters[counter];
}

DownloadQuery *BatchDownloader::query() const
{
	return m_query;
}


void BatchDownloader::start()
{
	// Resume download
	if (m_step == BatchDownloadStep::Aborted) {
		if (!m_imageDownloaders.isEmpty()) {
			setCurrentStep(BatchDownloadStep::ImageDownload);
			for (auto it = m_imageDownloaders.constBegin(); it != m_imageDownloaders.constEnd(); ++it) {
				it.value()->save();
			}
			return;
		} else if (m_packLoader != nullptr) {
			nextPack();
			return;
		}
	}

	// Invalid step
	else if (m_step != BatchDownloadStep::NotStarted) {
		return;
	}

	// Reset counters
	m_counters.clear();
	m_counterSum = 0;

	// Reset total
	auto *group = dynamic_cast<DownloadQueryGroup*>(m_query);
	m_totalCount = group != nullptr ? group->total : 1;

	//m_profile->getCommands().before();
	login();
}

void BatchDownloader::abort()
{
	setCurrentStep(BatchDownloadStep::Aborted);

	if (!m_imageDownloaders.isEmpty()) {
		for (auto it = m_imageDownloaders.constBegin(); it != m_imageDownloaders.constEnd(); ++it) {
			it.value()->abort();
		}
	} else if (m_packLoader != nullptr) {
		m_packLoader->abort();
	}
}

void BatchDownloader::login()
{
	setCurrentStep(BatchDownloadStep::Login);

	Site *site = m_query->site;
	connect(site, &Site::loggedIn, this, &BatchDownloader::loginFinished, Qt::QueuedConnection);
	site->login();
}

void BatchDownloader::loginFinished()
{
	disconnect(m_query->site, &Site::loggedIn, this, &BatchDownloader::loginFinished);

	auto *group = dynamic_cast<DownloadQueryGroup*>(m_query);
	if (group != nullptr) {
		bool usePacking = m_settings->value("packing_enable", true).toBool();
		int imagesPerPack = m_settings->value("packing_size", 1000).toInt();
		m_packLoader = new PackLoader(m_profile, *group, usePacking ? imagesPerPack : -1, this);
		m_packLoader->start();
		nextPack();
	} else {
		auto *img = dynamic_cast<DownloadQueryImage*>(m_query);
		m_pendingDownloads.append(img->image);
		nextImages();
	}
}

void BatchDownloader::nextPack()
{
	if (!m_packLoader->hasNext()) {
		allFinished();
		return;
	}

	setCurrentStep(BatchDownloadStep::PageDownload);

	int packSize = m_packLoader->nextPackSize();
	auto images = m_packLoader->next();

	// Check missing images from the pack (if we expected 1000 but only got 900, we should consider 100 missing)
	m_counters[Counter::Missing] += packSize - images.count();

	m_pendingDownloads.append(images);
	nextImages();
}

void BatchDownloader::nextImages()
{
	setCurrentStep(BatchDownloadStep::ImageDownload);

	// Start the simultaneous downloads
	int count = qMax(1, qMin(m_settings->value("Save/simultaneous").toInt(), 10));
	m_currentlyProcessing.store(count); // TODO: this should be shared amongst instances
	for (int i = 0; i < count; ++i) {
		nextImage();
	}
}

void BatchDownloader::nextImage()
{
	// We quit as soon as the user cancels
	if (m_step != BatchDownloadStep::ImageDownload) {
		return;
	}

	// If we already finished
	if (m_pendingDownloads.empty()) {
		if (m_currentlyProcessing.fetchAndAddRelaxed(-1) == 1) {
			allFinished();
		}
		return;
	}

	// We take the first image to download
	QSharedPointer<Image> img = m_pendingDownloads.dequeue();
	loadImage(img);
}

void BatchDownloader::loadImage(QSharedPointer<Image> img)
{
	// If there is already a downloader for this image, we simply restart it
	if (m_imageDownloaders.contains(img)) {
		m_imageDownloaders[img]->save();
		return;
	}

	// Path
	QString filename = m_query->filename;
	QString path = m_query->path;
	auto *group = dynamic_cast<DownloadQueryGroup*>(m_query);

	// Start loading and saving image
	int count = m_counterSum + 1;
	bool getBlacklisted = group == nullptr || group->getBlacklisted;
	auto imgDownloader = new ImageDownloader(m_profile, img, filename, path, count, true, false, this);
	if (!getBlacklisted) {
		imgDownloader->setBlacklist(&m_profile->getBlacklist());
	}
	connect(imgDownloader, &ImageDownloader::saved, this, &BatchDownloader::loadImageFinished, Qt::UniqueConnection);
	m_imageDownloaders[img] = imgDownloader;
	imgDownloader->save();
}

void BatchDownloader::loadImageFinished(const QSharedPointer<Image> &img, QList<ImageSaveResult> result)
{
	// Delete ImageDownloader to prevent leaks
	m_imageDownloaders[img]->deleteLater();
	m_imageDownloaders.remove(img);

	// Save error count to compare it later on
	bool diskError = false;
	const auto res = result.first().result;

	// Disk writing errors
	for (const ImageSaveResult &re : result) {
		if (re.result == Image::SaveResult::Error) {
			// TODO: detect disk error
			// TODO: report errors somehos
		}
	}

	// Increase counters
	if (res == Image::SaveResult::NetworkError) {
		m_counters[Counter::Errors]++;
		m_failedDownloads.append(img);
	} else if (res == Image::SaveResult::NotFound) {
		m_counters[Counter::NotFound]++;
	} else if (res == Image::SaveResult::AlreadyExistsDisk) {
		m_counters[Counter::AlreadyExists]++;
	} else if (res == Image::SaveResult::Blacklisted || res == Image::SaveResult::AlreadyExistsMd5 || res == Image::SaveResult::AlreadyExistsDeletedMd5) {
		m_counters[Counter::Ignored]++;
	} else if (!diskError) {
		m_counters[Counter::Downloaded]++;
	}

	// Start downloading the next image
	if (!diskError) {
		m_counterSum++;

		QCoreApplication::processEvents();
		QTimer::singleShot(0, this, SLOT(nextImage()));
	}
}

void BatchDownloader::allFinished()
{
	// If we didn't really finish yet
	if (m_packLoader != nullptr && m_packLoader->hasNext()) {
		nextPack();
		return;
	}

	// Cleanup
	if (m_packLoader != nullptr) {
		m_packLoader->deleteLater();
		m_packLoader = nullptr;
	}

	//m_profile->getCommands().after();
	setCurrentStep(BatchDownloadStep::Finished);

	emit finished();
}
