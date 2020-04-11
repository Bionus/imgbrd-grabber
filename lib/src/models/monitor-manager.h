#ifndef MONITOR_MANAGER_H
#define MONITOR_MANAGER_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QString>


class Monitor;
class Site;

class MonitorManager : public QObject
{
	Q_OBJECT

	public:
		MonitorManager(QString file, const QMap<QString, Site*> &sites);
		void add(const Monitor &monitor);
		void remove(const Monitor &monitor);
		const QList<Monitor> &monitors() const;

	private:
		void load();
		void save() const;

	signals:
		void changed();

	private:
		QString m_file;
		const QMap<QString, Site*> &m_sites;
		QList<Monitor> m_monitors;
};

#endif // MONITOR_MANAGER_H
