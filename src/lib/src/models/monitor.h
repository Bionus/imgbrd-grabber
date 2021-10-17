#ifndef MONITOR_H
#define MONITOR_H

#include <QDateTime>
#include <QList>
#include "models/search-query/search-query.h"


class QJsonObject;
class Site;

class Monitor
{
	public:
		Monitor(QList<Site*> sites, int interval, QDateTime lastCheck, bool download, QString pathOverride, QString filenameOverride, int cumulated = 0, bool preciseCumulated = true, SearchQuery query = {}, QStringList postFilters = {}, bool notify = true, int delay = 0, bool getBlacklisted = false, QString lastState = {}, QDateTime lastStateSince = {}, int lastStateCount = 0);
		qint64 secsToNextCheck() const;

		// Getters and setters
		QList<Site*> sites() const;
		int interval() const;
		int delay() const;
		const QDateTime &lastCheck() const;
		void setLastCheck(const QDateTime &lastCheck);
		int cumulated() const;
		bool preciseCumulated() const;
		void setCumulated(int cumulated, bool isPrecise);
		bool download() const;
		const QString &pathOverride() const;
		const QString &filenameOverride() const;
		const SearchQuery &query() const;
		const QStringList &postFilters() const;
		bool notify() const;
		bool getBlacklisted() const;
		const QString &lastState() const;
		const QDateTime &lastStateSince() const;
		int lastStateCount() const;
		void setLastState(const QString &lastState);
		void setForceRun();

		// Serialization
		void toJson(QJsonObject &json) const;
		static Monitor fromJson(const QJsonObject &json, Profile *profile);

	private:
		QList<Site*> m_sites;
		int m_interval; // In seconds
		int m_delay; // In seconds
		QDateTime m_lastCheck;
		int m_cumulated;
		bool m_preciseCumulated;
		bool m_download;
		QString m_pathOverride;
		QString m_filenameOverride;
		SearchQuery m_query;
		QStringList m_postFilters;
		bool m_notify;
		bool m_getBlacklisted;
		QString m_lastState;
		QDateTime m_lastStateSince;
		int m_lastStateCount;
		bool m_forceRun = false;
};

bool operator==(const Monitor &lhs, const Monitor &rhs);
bool operator!=(const Monitor &lhs, const Monitor &rhs);

#endif // MONITORING_H
