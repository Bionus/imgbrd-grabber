#ifndef MONITOR_H
#define MONITOR_H

#include <QDateTime>
#include <QJsonObject>


class Site;

class Monitor
{
	public:
		Monitor(Site *site, int interval, const QDateTime &lastCheck, int cumulated = 0, bool preciseCumulated = true);
		qint64 secsToNextCheck() const;

		// Getters and setters
		Site *site() const;
		int interval() const;
		const QDateTime &lastCheck() const;
		void setLastCheck(const QDateTime &lastCheck);
		int cumulated() const;
		bool preciseCumulated() const;
		void setCumulated(int cumulated, bool isPrecise);

		// Serialization
		void toJson(QJsonObject &json) const;
		static Monitor fromJson(const QJsonObject &json, const QMap<QString, Site*> &sites);

	private:
		Site *m_site;
		int m_interval;
		QDateTime m_lastCheck;
		int m_cumulated;
		bool m_preciseCumulated;
};

#endif // MONITORING_H
