#ifndef MONITORING_CENTER_H
#define MONITORING_CENTER_H

#include <QObject>


class Favorite;
class ImageDownloader;
class Monitor;
class Profile;
class QSystemTrayIcon;

class MonitoringCenter : public QObject
{
	Q_OBJECT

	public:
		explicit MonitoringCenter(Profile *profile, QSystemTrayIcon *trayIcon, QObject *parent = nullptr);

	public slots:
		void start();
		void stop();

	protected slots:
		void tick();
        void startDownload();
        void downloadFinished();

	protected:
		void checkMonitor(Monitor &monitor, const Favorite &favorite);

	private:
		Profile *m_profile;
		QSystemTrayIcon *m_trayIcon;
		bool m_stop = false;
        QList<ImageDownloader*> m_downloadQueue;
        bool m_downloading = false;
};

#endif // MONITORING_CENTER_H
