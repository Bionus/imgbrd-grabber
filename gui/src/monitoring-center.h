#ifndef MONITORING_CENTER_H
#define MONITORING_CENTER_H

#include <QObject>
#include <QSystemTrayIcon>


class Profile;

class MonitoringCenter : public QObject
{
	Q_OBJECT

	public:
		explicit MonitoringCenter(Profile *profile, QSystemTrayIcon *trayIcon, QObject *parent = Q_NULLPTR);

	public slots:
		void start();
		void stop();

	protected slots:
		void tick();

	private:
		Profile *m_profile;
		QSystemTrayIcon *m_trayIcon;
		bool m_stop = false;
};

#endif // MONITORING_CENTER_H
