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
{}

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
	Site *site = monitor.site();

	log(QStringLiteral("Monitoring new images for '%1' on '%2'").arg(search.toString(), site->name()), Logger::Info);

	// Create a pack loader
	DownloadQueryGroup query(m_profile->getSettings(), search, 1, MONITOR_CHECK_LIMIT, MONITOR_CHECK_TOTAL, postFiltering, site);
	PackLoader loader(m_profile, query, MONITOR_CHECK_LIMIT, this);
	loader.start();

	// Load all images
	bool firstRun = true;
	int count = 0;
	int newImages = 0;
	QList<QSharedPointer<Image>> newImagesList;
	while ((firstRun || monitor.download()) && loader.hasNext() && newImages == count) {
		// Load the next page
		QList<QSharedPointer<Image>> allImages = loader.next();
		count += allImages.count();

		// Filter out old images
		for (const QSharedPointer<Image> &img : allImages) {
			if (img->createdAt() > monitor.lastCheck()) {
				QStringList detected = m_profile->getBlacklist().match(img->tokens(m_profile));
				if (detected.isEmpty()) {
					newImagesList.append(img);
					newImages++;
				}
			}
		}
	}

	// Send notification
	if (newImages > 0 && m_trayIcon != nullptr && m_trayIcon->isVisible()) {
		QString msg;
		if (count == 1) {
			msg = tr("New images found for tag '%1' on '%2'");
		} else if (newImages < count) {
			msg = tr("%n new image(s) found for tag '%1' on '%2'", "", newImages);
		} else {
			msg = tr("More than %n new image(s) found for tag '%1' on '%2'", "", newImages);
		}
		m_trayIcon->showMessage(tr("Grabber monitoring"), msg.arg(search.toString(), site->name()), QSystemTrayIcon::Information);
	}

    // Add images to download queue
    if (monitor.download()) {
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
    }

	// Update monitor
	monitor.setLastCheck(QDateTime::currentDateTimeUtc());
	monitor.setCumulated(monitor.cumulated() + newImages, count != 1 && newImages < count);

	return newImages > 0;
}

void MonitoringCenter::tick()
{
	if (m_stop) {
		return;
	}

    qint64 minNextMonitoring = -1;
	log(QStringLiteral("Monitoring tick"), Logger::Info);

	// Favorites
	for (Favorite &fav : m_profile->getFavorites()) {
		for (Monitor &monitor : fav.getMonitors()) {
			// If this favorite's monitoring expired, we check it for updates
            qint64 next = monitor.secsToNextCheck();
			if (next <= 0) {
				checkMonitor(monitor, fav);
				next = monitor.secsToNextCheck();
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
			checkMonitor(monitor, monitor.query(), QStringList());
			next = monitor.secsToNextCheck();
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

void MonitoringCenter::stop()
{
	m_stop = true;

	log(QStringLiteral("Monitoring stopped"), Logger::Info);
}
