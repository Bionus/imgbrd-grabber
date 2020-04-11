#include "models/monitor.h"
#include <QMap>
#include <utility>
#include "models/profile.h"
#include "models/search-query/search-query.h"
#include "models/site.h"


Monitor::Monitor(Site *site, int interval, QDateTime lastCheck, bool download, QString pathOverride, QString filenameOverride, int cumulated, bool preciseCumulated, SearchQuery query)
    : m_site(site), m_interval(interval), m_lastCheck(std::move(lastCheck)), m_cumulated(cumulated), m_preciseCumulated(preciseCumulated), m_download(download), m_pathOverride(std::move(pathOverride)), m_filenameOverride(std::move(filenameOverride))
{}

qint64 Monitor::secsToNextCheck() const
{
	auto now = QDateTime::currentDateTimeUtc();
	return now.secsTo(m_lastCheck.addSecs(m_interval));
}



Site *Monitor::site() const
{
	return m_site;
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


void Monitor::toJson(QJsonObject &json) const
{
	json["site"] = m_site->url();
	json["interval"] = m_interval;
	json["lastCheck"] = m_lastCheck.toString(Qt::ISODate);
	json["cumulated"] = m_cumulated;
    json["preciseCumulated"] = m_preciseCumulated;
    json["download"] = m_download;
	json["pathOverride"] = m_pathOverride;
	json["filenameOverride"] = m_filenameOverride;

	QJsonObject jsonQuery;
	m_query.write(jsonQuery);
	json["query"] = jsonQuery;
}

Monitor Monitor::fromJson(const QJsonObject &json, Profile *profile)
{
	const QMap<QString, Site*> &sites = profile->getSites();
	Site *site = sites.value(json["site"].toString());

	const int interval = json["interval"].toInt();
	const QDateTime lastCheck = QDateTime::fromString(json["lastCheck"].toString(), Qt::ISODate);
	const int cumulated = json["cumulated"].toInt();
	const bool preciseCumulated = json["preciseCumulated"].toBool();
    const bool download = json["download"].toBool();
    const QString &pathOverride = json["pathOverride"].toString();
    const QString &filenameOverride = json["filenameOverride"].toString();
	SearchQuery query;
	query.read(json["query"].toObject(), profile);
	return Monitor(site, interval, lastCheck, download, pathOverride, filenameOverride, cumulated, preciseCumulated, query);
}


bool operator==(const Monitor &lhs, const Monitor &rhs)
{
	return lhs.site() == rhs.site()
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
