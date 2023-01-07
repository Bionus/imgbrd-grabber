#include "search-images-cli-command.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QSet>
#include <QSettings>
#include <utility>
#include "downloader/download-query-group.h"
#include "loader/pack-loader.h"
#include "logger.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"


SearchImagesCliCommand::SearchImagesCliCommand(Profile *profile, QStringList tags, QStringList postFiltering, QList<Site*> sites, int page, int perPage, QString filename, QString folder, int max, bool login, bool noDuplicates, bool getBlacklisted, QObject *parent)
	: SearchCliCommand(profile, std::move(tags), std::move(postFiltering), std::move(sites), page, perPage, parent), m_filename(filename), m_folder(folder), m_max(max), m_login(login), m_noDuplicates(noDuplicates), m_getBlacklisted(getBlacklisted)
{}

bool SearchImagesCliCommand::validate()
{
	if (m_sites.isEmpty()) {
		log("You must provide at least one source to load the images from", Logger::Error);
		return false;
	}

	if (m_perPage <= 0) {
		log("The number of images per page must be more than 0", Logger::Error);
		return false;
	}

	if (m_max <= 0) {
		log("The image limit must be more than 0", Logger::Error);
		return false;
	}

	return true;
}

QList<QSharedPointer<Image>> SearchImagesCliCommand::getAllImages()
{
	const bool usePacking = m_profile->getSettings()->value("packing_enable", true).toBool();
	const int imagesPerPack = m_profile->getSettings()->value("packing_size", 1000).toInt();
	const int packSize = usePacking ? imagesPerPack : -1;

	QSet<QString> md5s;
	QList<QSharedPointer<Image>> images;

	for (auto *site : m_sites) {
		DownloadQueryGroup query(m_tags, m_page, m_perPage, m_max, m_postFiltering, m_getBlacklisted, site, m_filename, m_folder);

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

void SearchImagesCliCommand::loadMoreDetails(const QList<QSharedPointer<Image>> &images)
{
	if (images.isEmpty()) {
		return;
	}

	int work = images.length();
	int requestsLimit = 5; // Simultaneous requests
	int runningRequests = 0;

	QEventLoop loop;
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
