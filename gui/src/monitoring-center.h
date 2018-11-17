#ifndef MONITORING_CENTER_H
#define MONITORING_CENTER_H

#include <QObject>


class Favorite;
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

	protected:
		void checkMonitor(Monitor &monitor, const Favorite &favorite);

	private:
		Profile *m_profile;
		QSystemTrayIcon *m_trayIcon;
		bool m_stop = false;
};

#endif // MONITORING_CENTER_H
