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

	public:
		explicit MonitoringCenter(Profile *profile, DownloadQueue *downloadQueue, QSystemTrayIcon *trayIcon, QObject *parent = nullptr);

	public slots:
		void start();
		void stop();

	protected slots:
		void tick();

	protected:
		void checkMonitor(Monitor &monitor, const Favorite &favorite);
		bool checkMonitor(Monitor &monitor, const SearchQuery &search, const QStringList &postFiltering);

	private:
		Profile *m_profile;
		DownloadQueue *m_downloadQueue;
		QSystemTrayIcon *m_trayIcon;
		bool m_stop = false;
};

#endif // MONITORING_CENTER_H
