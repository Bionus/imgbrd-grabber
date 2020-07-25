#ifndef MONITORING_CENTER_H
#define MONITORING_CENTER_H

#include <QObject>


class DownloadQueue;
class Favorite;
class ImageDownloader;
class Monitor;
class Profile;
class QSystemTrayIcon;
class SearchQuery;

class MonitoringCenter : public QObject
{
	Q_OBJECT
	Q_ENUMS(LoadResult)

	public:
		enum MonitoringStatus
		{
			Waiting,
			Checking,
			Performing,
		};

		explicit MonitoringCenter(Profile *profile, DownloadQueue *downloadQueue, QSystemTrayIcon *trayIcon, QObject *parent = nullptr);
		bool isRunning() const;

	public slots:
		void start();
		void stop();

	protected slots:
		void tick();
		void queueEmpty();

	protected:
		void checkMonitor(Monitor &monitor, const Favorite &favorite);
		bool checkMonitor(Monitor &monitor, const SearchQuery &search, const QStringList &postFiltering);

	signals:
		void statusChanged(const Monitor &monitor, MonitoringCenter::MonitoringStatus status);

	private:
		Profile *m_profile;
		DownloadQueue *m_downloadQueue;
		QSystemTrayIcon *m_trayIcon;
		bool m_waitingForQueue = false;
		bool m_changed = false;
		bool m_stop = false;
};

#endif // MONITORING_CENTER_H
