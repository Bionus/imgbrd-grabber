#include "monitoring-center.h"
#include <QEventLoop>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTimer>
#include "downloader/download-query-group.h"
#include "downloader/download-queue.h"
#include "downloader/image-downloader.h"
#include "loader/pack-loader.h"
#include "logger.h"
#include "models/favorite.h"
#include "models/image.h"
#include "models/monitor.h"
#include "models/monitor-manager.h"
#include "models/profile.h"
#include "models/search-query/tag-search-query.h"
#include "models/site.h"

#define MONITOR_CHECK_LIMIT 20
#define MONITOR_CHECK_TOTAL 1000


MonitoringCenter::MonitoringCenter(Profile *profile, DownloadQueue *downloadQueue, QSystemTrayIcon *trayIcon, QObject *parent)
	: QObject(parent), m_profile(profile), m_downloadQueue(downloadQueue), m_trayIcon(trayIcon)
{
	connect(m_downloadQueue, &DownloadQueue::finished, this, &MonitoringCenter::queueEmpty);
}

void MonitoringCenter::start()
{
	log(QStringLiteral("Monitoring starting"), Logger::Info);

	m_stop = false;
	int secsDelay = m_profile->getSettings()->value("Monitoring/startupDelay", 0).toInt();
	QTimer::singleShot(secsDelay * 1000, this, SLOT(tick()));
}

void MonitoringCenter::checkMonitor(Monitor &monitor, const Favorite &favorite)
{
	bool newImages = checkMonitor(monitor, favorite.getName().split(' ', QString::SkipEmptyParts), favorite.getPostFiltering());
	if (newImages) {
		emit m_profile->favoritesChanged();
	}
}

bool MonitoringCenter::checkMonitor(Monitor &monitor, const SearchQuery &search, const QStringList &postFiltering)
{
	const int delay = monitor.delay();
	const QDateTime limit = QDateTime::currentDateTimeUtc().addSecs(-delay);

	QStringList siteNames;
	for (Site *site : monitor.sites()) {
		siteNames.append(site->name());
	}

	emit statusChanged(monitor, MonitoringStatus::Checking);
	log(QStringLiteral("Monitoring new images for '%1' on '%2'").arg(search.toString(), siteNames.join(", ")), Logger::Info);

	int count = 0;
	int newImages = 0;
	QList<QSharedPointer<Image>> newImagesList;

	for (Site *site : monitor.sites()) {
		// Create a pack loader
		DownloadQueryGroup query(m_profile->getSettings(), search, 1, MONITOR_CHECK_LIMIT, MONITOR_CHECK_TOTAL, postFiltering, site);
		PackLoader loader(m_profile, query, MONITOR_CHECK_LIMIT, this);
		loader.start();

		// Load all images
		bool firstRun = true;
		int countRun = 0;
		int newImagesRun = 0;
		while ((firstRun || monitor.download()) && loader.hasNext() && newImagesRun == countRun) {
			// Load the next page
			QList<QSharedPointer<Image>> allImages = loader.next();
			countRun += allImages.count();

			// Filter out old images
			for (const QSharedPointer<Image> &img : allImages) {
				if (img->createdAt() > monitor.lastCheck() && (delay <= 0 || img->createdAt() <= limit)) {
					QStringList detected = m_profile->getBlacklist().match(img->tokens(m_profile));
					if (detected.isEmpty()) {
						newImagesList.append(img);
						newImagesRun++;
					}
				}
			}
		}

		count += countRun;
		newImages += newImagesRun;
	}

	emit statusChanged(monitor, MonitoringStatus::Performing);

	// Send notification
	if (monitor.notify() && newImages > 0 && m_trayIcon != nullptr && m_trayIcon->isVisible()) {
		QString msg;
		if (count == 1) {
			msg = tr("New images found for tag '%1' on '%2'");
		} else if (newImages < count) {
			msg = tr("%n new image(s) found for tag '%1' on '%2'", "", newImages);
		} else {
			msg = tr("More than %n new image(s) found for tag '%1' on '%2'", "", newImages);
		}
		m_trayIcon->showMessage(tr("Grabber monitoring"), msg.arg(search.toString(), siteNames.join(", ")), QSystemTrayIcon::Information);
	}

    // Add images to download queue
	if (monitor.download() && newImages > 0) {
        QString path = monitor.pathOverride();
        if (path.isEmpty()) {
            path = m_profile->getSettings()->value("save/path").toString();
        }
        QString filename = monitor.filenameOverride();
        if (filename.isEmpty()) {
            filename = m_profile->getSettings()->value("save/filename").toString();
        }

        for (const QSharedPointer<Image> &img : newImagesList) {
			auto downloader = new ImageDownloader(m_profile, img, filename, path, 0, true, false, this);
			m_downloadQueue->add(DownloadQueue::Background, downloader);
		}

		log(QStringLiteral("Added %1 images to the download queue for monitor '%2' on '%3'").arg(QString::number(newImages), search.toString(), siteNames.join(", ")), Logger::Info);
		m_waitingForQueue = true;
    }

	// Update monitor
	monitor.setLastCheck(limit);
	monitor.setCumulated(monitor.cumulated() + newImages, count != 1 && newImages < count);
	m_changed = true;

	emit statusChanged(monitor, MonitoringStatus::Waiting);

	return newImages > 0;
}

void MonitoringCenter::tick()
{
	if (m_stop) {
		return;
	}

    qint64 minNextMonitoring = -1;
	log(QStringLiteral("Monitoring tick"), Logger::Info);

	// Save if there were changes to the monitors since the last tick
	if (m_changed) {
		m_profile->syncFavorites();
		m_profile->monitorManager()->save();
		m_changed = false;
	}

	// Favorites
	QList<Favorite> &favs = m_profile->getFavorites();
	for (int j = 0; j < favs.count(); ++j) {
		Favorite &fav = favs[j];
		for (Monitor &monitor : fav.getMonitors()) {
			// If this favorite's monitoring expired, we check it for updates
            qint64 next = monitor.secsToNextCheck();
			if (next <= 0) {
				checkMonitor(monitor, fav);
				next = monitor.secsToNextCheck();
			}

			if (m_waitingForQueue) {
				return;
			}

			// Only keep the soonest expiring timeout
			if (next < minNextMonitoring || minNextMonitoring == -1) {
				minNextMonitoring = next;
			}
		}
	}

	// Normal monitors
	for (Monitor &monitor : m_profile->monitorManager()->monitors()) {
		// If this favorite's monitoring expired, we check it for updates
		qint64 next = monitor.secsToNextCheck();
		if (next <= 0) {
			checkMonitor(monitor, monitor.query(), monitor.postFilters());
			next = monitor.secsToNextCheck();
		}

		if (m_waitingForQueue) {
			return;
		}

		// Only keep the soonest expiring timeout
		if (next < minNextMonitoring || minNextMonitoring == -1) {
			minNextMonitoring = next;
		}
	}

	// Re-run this method as soon as one of the monitoring timeout expires
	if (minNextMonitoring > 0) {
		log(QStringLiteral("Next monitoring will be in %1 seconds").arg(minNextMonitoring), Logger::Info);
		QTimer::singleShot(minNextMonitoring * 1000, this, SLOT(tick()));
	} else {
		log(QStringLiteral("Monitoring finished"), Logger::Info);
	}
}

void MonitoringCenter::queueEmpty()
{
	if (m_waitingForQueue) {
		m_waitingForQueue = false;
		tick();
	}
}

void MonitoringCenter::stop()
{
	m_stop = true;

	log(QStringLiteral("Monitoring stopped"), Logger::Info);
}
