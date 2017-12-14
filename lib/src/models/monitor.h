#ifndef MONITOR_H
#define MONITOR_H

#include <QDateTime>
#include <QJsonObject>
#include <QMap>


class Site;

class Monitor
{
	public:
		Monitor(Site *site, int interval, const QDateTime &lastCheck);
		int secsToNextCheck() const;

		// Getters and setters
		Site *site() const;
		int interval() const;
		const QDateTime &lastCheck() const;
		void setLastCheck(const QDateTime &lastCheck);

		// Serialization
		void toJson(QJsonObject &json) const;
		static Monitor fromJson(const QJsonObject &json, const QMap<QString, Site*> &sites);

	private:
		Site *m_site;
		int m_interval;
		QDateTime m_lastCheck;
};

#endif // MONITORING_H
