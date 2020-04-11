#ifndef MONITOR_H
#define MONITOR_H

#include <QDateTime>
#include <QJsonObject>
#include "models/search-query/search-query.h"


class Site;

class Monitor
{
	public:
		Monitor(Site *site, int interval, QDateTime lastCheck, bool download, QString pathOverride, QString filenameOverride, int cumulated = 0, bool preciseCumulated = true, SearchQuery query = {});
		qint64 secsToNextCheck() const;

		// Getters and setters
		Site *site() const;
		int interval() const;
		const QDateTime &lastCheck() const;
		void setLastCheck(const QDateTime &lastCheck);
		int cumulated() const;
		bool preciseCumulated() const;
		void setCumulated(int cumulated, bool isPrecise);
        bool download() const;
        const QString &pathOverride() const;
		const QString &filenameOverride() const;
		const SearchQuery &query() const;

		// Serialization
		void toJson(QJsonObject &json) const;
		static Monitor fromJson(const QJsonObject &json, Profile *profile);

	private:
		Site *m_site;
		int m_interval;
		QDateTime m_lastCheck;
		int m_cumulated;
		bool m_preciseCumulated;
        bool m_download;
        QString m_pathOverride;
        QString m_filenameOverride;
		SearchQuery m_query;
};

bool operator==(const Monitor &lhs, const Monitor &rhs);
bool operator!=(const Monitor &lhs, const Monitor &rhs);

#endif // MONITORING_H
