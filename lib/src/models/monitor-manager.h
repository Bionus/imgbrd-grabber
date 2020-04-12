#ifndef MONITOR_MANAGER_H
#define MONITOR_MANAGER_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QString>


class Monitor;
class Profile;

class MonitorManager : public QObject
{
	Q_OBJECT

	public:
		MonitorManager(QString file, Profile *profile);
		void add(const Monitor &monitor);
		void remove(const Monitor &monitor);
		QList<Monitor> &monitors();

	private:
		void load();
		void save() const;

	signals:
		void changed();

	private:
		QString m_file;
		Profile *m_profile;
		QList<Monitor> m_monitors;
};

#endif // MONITOR_MANAGER_H
