#include "monitoring-center.h"
#include <QEventLoop>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTimer>
#include "logger.h"
#include "models/favorite.h"
#include "models/image.h"
#include "models/monitor.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"

#define MONITOR_CHECK_LIMIT 20


MonitoringCenter::MonitoringCenter(Profile *profile, QSystemTrayIcon *trayIcon, QObject *parent)
	: QObject(parent), m_profile(profile), m_trayIcon(trayIcon)
{ }

void MonitoringCenter::start()
{
	log(QStringLiteral("Monitoring starting"), Logger::Info);

	m_stop = false;
	int secsDelay = m_profile->getSettings()->value("Monitoring/startupDelay", 0).toInt();
	QTimer::singleShot(secsDelay * 1000, this, SLOT(tick()));
}

void MonitoringCenter::checkMonitor(Monitor &monitor, const Favorite &favorite)
{
	Site *site = monitor.site();

	// Load the last page to check for new images
	QEventLoop loop;
	Page *page = new Page(m_profile, site, m_profile->getSites().values(), favorite.getName().split(' '), 1, MONITOR_CHECK_LIMIT, QStringList(), false, this);
	connect(page, &Page::finishedLoading, &loop, &QEventLoop::quit);
	page->load();
	loop.exec();

	// Count new images
	int newImages = 0;
	int count = page->images().count();
	for (const QSharedPointer<Image> &img : page->images())
	{
		if (img->createdAt() > monitor.lastCheck())
		{ newImages++; }
	}

	// Send notification
	if (newImages > 0 && m_trayIcon != nullptr && m_trayIcon->isVisible())
	{
		QString msg;
		if (count == 1)
		{ msg = tr("New images found for tag '%1' on '%2'"); }
		else if (newImages < count)
		{ msg = tr("%n new image(s) found for tag '%1' on '%2'", "", newImages); }
		else
		{ msg = tr("More than %n new image(s) found for tag '%1' on '%2'", "", newImages); }
		m_trayIcon->showMessage(tr("Grabber monitoring"), msg.arg(favorite.getName(), site->name()), QSystemTrayIcon::Information);
	}

	// Update monitor
	monitor.setLastCheck(QDateTime::currentDateTimeUtc());
	monitor.setCumulated(monitor.cumulated() + newImages, count != 1 && newImages < count);

	if (newImages > 0)
	{ emit m_profile->favoritesChanged(); }
}

void MonitoringCenter::tick()
{
	if (m_stop)
		return;

	int minNextMonitoring = -1;
	log(QStringLiteral("Monitoring tick"), Logger::Info);

	for (Favorite &fav : m_profile->getFavorites())
	{
		for (Monitor &monitor : fav.getMonitors())
		{
			// If this favorite's monitoring expired, we check it for updates
			int next = monitor.secsToNextCheck();
			if (next <= 0)
			{
				checkMonitor(monitor, fav);
				next = monitor.secsToNextCheck();
			}

			// Only keep the soonest expiring timeout
			if (next < minNextMonitoring || minNextMonitoring == -1)
			{ minNextMonitoring = next; }
		}
	}

	// Re-run this method as soon as one of the monitoring timeout expires
	if (minNextMonitoring > 0)
	{
		log(QStringLiteral("Next monitoring will be in %1 seconds").arg(minNextMonitoring), Logger::Info);
		QTimer::singleShot(minNextMonitoring * 1000, this, SLOT(tick()));
	}
	else
	{ log(QStringLiteral("Monitoring finished"), Logger::Info); }
}

void MonitoringCenter::stop()
{
	m_stop = true;

	log(QStringLiteral("Monitoring stopped"), Logger::Info);
}
