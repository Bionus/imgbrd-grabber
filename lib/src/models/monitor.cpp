#include "models/monitor.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <utility>
#include "models/profile.h"
#include "models/search-query/search-query.h"
#include "models/site.h"


Monitor::Monitor(QList<Site *> sites, int interval, QDateTime lastCheck, bool download, QString pathOverride, QString filenameOverride, int cumulated, bool preciseCumulated, SearchQuery query, QStringList postFilters, bool notify)
	: m_sites(sites), m_interval(interval), m_lastCheck(std::move(lastCheck)), m_cumulated(cumulated), m_preciseCumulated(preciseCumulated), m_download(download), m_pathOverride(std::move(pathOverride)), m_filenameOverride(std::move(filenameOverride)), m_query(query), m_postFilters(postFilters), m_notify(notify)
{}

qint64 Monitor::secsToNextCheck() const
{
	auto now = QDateTime::currentDateTimeUtc();
	return now.secsTo(m_lastCheck.addSecs(m_interval));
}



QList<Site*> Monitor::sites() const
{
	return m_sites;
}

int Monitor::interval() const
{
	return m_interval;
}

const QDateTime &Monitor::lastCheck() const
{
	return m_lastCheck;
}
void Monitor::setLastCheck(const QDateTime &lastCheck)
{
	m_lastCheck = lastCheck;
}

int Monitor::cumulated() const
{
	return m_cumulated;
}
bool Monitor::preciseCumulated() const
{
	return m_preciseCumulated;
}
void Monitor::setCumulated(int cumulated, bool isPrecise)
{
	m_cumulated = cumulated;
	m_preciseCumulated = isPrecise;
}

bool Monitor::download() const
{
    return m_download;
}
const QString &Monitor::pathOverride() const
{
    return m_pathOverride;
}
const QString &Monitor::filenameOverride() const
{
    return m_filenameOverride;
}
const SearchQuery &Monitor::query() const
{
	return m_query;
}
const QStringList &Monitor::postFilters() const
{
	return m_postFilters;
}
bool Monitor::notify() const
{
	return m_notify;
}


void Monitor::toJson(QJsonObject &json) const
{
	QStringList sites;
	for (auto site : m_sites) {
		sites.append(site->url());
	}
	json["sites"] = QJsonArray::fromStringList(sites);

	json["interval"] = m_interval;
	json["lastCheck"] = m_lastCheck.toString(Qt::ISODate);
	json["cumulated"] = m_cumulated;
    json["preciseCumulated"] = m_preciseCumulated;
    json["download"] = m_download;
	json["pathOverride"] = m_pathOverride;
	json["filenameOverride"] = m_filenameOverride;
	json["postFilters"] = QJsonArray::fromStringList(m_postFilters);
	json["notify"] = m_notify;

	QJsonObject jsonQuery;
	m_query.write(jsonQuery);
	json["query"] = jsonQuery;
}

Monitor Monitor::fromJson(const QJsonObject &json, Profile *profile)
{
	const QMap<QString, Site*> &siteMap = profile->getSites();

	QList<Site*> sites;
	QJsonArray jsonSites = json["sites"].toArray();
	for (auto site : jsonSites) {
		sites.append(siteMap.value(site.toString()));
	}
	if (json.contains("site")) {
		sites.append(siteMap.value(json["site"].toString()));
	}

	const int interval = json["interval"].toInt();
	const QDateTime lastCheck = QDateTime::fromString(json["lastCheck"].toString(), Qt::ISODate);
	const int cumulated = json["cumulated"].toInt();
	const bool preciseCumulated = json["preciseCumulated"].toBool();
    const bool download = json["download"].toBool();
	const QString &pathOverride = json["pathOverride"].toString();
	const QString &filenameOverride = json["filenameOverride"].toString();
	const bool notify = json["notify"].toBool();

	QStringList postFilters;
	QJsonArray jsonPostFilters = json["postFilters"].toArray();
	for (auto filter : jsonPostFilters) {
		postFilters.append(filter.toString());
	}

	SearchQuery query;
	query.read(json["query"].toObject(), profile);

	return Monitor(sites, interval, lastCheck, download, pathOverride, filenameOverride, cumulated, preciseCumulated, query, postFilters, notify);
}


bool operator==(const Monitor &lhs, const Monitor &rhs)
{
	return lhs.sites() == rhs.sites()
		&& lhs.interval() == rhs.interval()
		&& lhs.lastCheck() == rhs.lastCheck()
		&& lhs.cumulated() == rhs.cumulated()
        && lhs.preciseCumulated() == rhs.preciseCumulated()
        && lhs.download() == rhs.download()
        && lhs.pathOverride() == rhs.pathOverride()
        && lhs.filenameOverride() == rhs.filenameOverride();
}

bool operator!=(const Monitor &lhs, const Monitor &rhs)
{ return !(lhs == rhs); }
