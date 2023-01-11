#include "monitoring/monitor.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <utility>
#include "logger.h"
#include "models/profile.h"
#include "models/search-query/search-query.h"
#include "models/site.h"


Monitor::Monitor(QList<Site *> sites, int interval, QDateTime lastCheck, bool download, QString pathOverride, QString filenameOverride, int cumulated, bool preciseCumulated, SearchQuery query, QStringList postFilters, bool notify, int delay, bool getBlacklisted, QString lastState, QDateTime lastStateSince, int lastStateCount)
	: m_sites(std::move(sites)), m_interval(interval), m_delay(delay), m_lastCheck(std::move(lastCheck)), m_cumulated(cumulated), m_preciseCumulated(preciseCumulated), m_download(download), m_pathOverride(std::move(pathOverride)), m_filenameOverride(std::move(filenameOverride)), m_query(std::move(query)), m_postFilters(std::move(postFilters)), m_notify(notify), m_getBlacklisted(getBlacklisted), m_lastState(std::move(lastState)), m_lastStateSince(std::move(lastStateSince)), m_lastStateCount(lastStateCount)
{}

qint64 Monitor::secsToNextCheck() const
{
	if (m_forceRun) {
		return -1;
	}

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

int Monitor::delay() const
{
	return m_delay;
}

const QDateTime &Monitor::lastCheck() const
{
	return m_lastCheck;
}
void Monitor::setLastCheck(const QDateTime &lastCheck)
{
	m_lastCheck = lastCheck;
	m_forceRun = false;
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
bool Monitor::getBlacklisted() const
{
	return m_getBlacklisted;
}

const QString &Monitor::lastState() const
{
	return m_lastState;
}
const QDateTime &Monitor::lastStateSince() const
{
	return m_lastStateSince;
}
int Monitor::lastStateCount() const
{
	return m_lastStateCount;
}
void Monitor::setLastState(const QString &lastState)
{
	if (m_lastState == lastState) {
		m_lastStateCount++;
		if (m_lastStateSince.isNull() || !m_lastStateSince.isValid()) {
			m_lastStateSince = QDateTime::currentDateTimeUtc();
		}
	} else {
		m_lastState = lastState;
		m_lastStateCount = 1;
		m_lastStateSince = QDateTime::currentDateTimeUtc();
	}
}

void Monitor::setForceRun()
{
	m_forceRun = true;
}


void Monitor::toJson(QJsonObject &json) const
{
	QStringList sites;
	for (auto site : m_sites) {
		sites.append(site->url());
	}
	json["sites"] = QJsonArray::fromStringList(sites);

	json["interval"] = m_interval;
	json["delay"] = m_delay;
	json["lastCheck"] = m_lastCheck.toString(Qt::ISODate);
	json["cumulated"] = m_cumulated;
	json["preciseCumulated"] = m_preciseCumulated;
	json["download"] = m_download;
	json["pathOverride"] = m_pathOverride;
	json["filenameOverride"] = m_filenameOverride;
	json["postFilters"] = QJsonArray::fromStringList(m_postFilters);
	json["notify"] = m_notify;
	json["getBlacklisted"] = m_getBlacklisted;

	QJsonObject lastState;
	lastState["state"] = m_lastState;
	lastState["since"] = m_lastStateSince.toString(Qt::ISODate);
	lastState["count"] = m_lastStateCount;
	json["lastState"] = lastState;

	QJsonObject jsonQuery;
	m_query.write(jsonQuery);
	json["query"] = jsonQuery;
}

Monitor Monitor::fromJson(const QJsonObject &json, Profile *profile)
{
	const QMap<QString, Site*> &siteMap = profile->getSites();

	QList<Site*> sites;
	QJsonArray jsonSites = json["sites"].toArray();
	for (auto jsonSite : jsonSites) {
		const QString site = jsonSite.toString();
		if (siteMap.contains(site)) {
			sites.append(siteMap.value(site));
		} else {
			log(QStringLiteral("Invalid source found for monitor: %1").arg(site), Logger::Error);
		}
	}
	if (json.contains("site")) {
		const QString site = json["site"].toString();
		if (siteMap.contains(site)) {
			sites.append(siteMap.value(site));
		} else {
			log(QStringLiteral("Invalid source found for monitor: %1").arg(site), Logger::Error);
		}
	}

	const int interval = json["interval"].toInt();
	const int delay = json["delay"].toInt();
	const QDateTime lastCheck = QDateTime::fromString(json["lastCheck"].toString(), Qt::ISODate);
	const int cumulated = json["cumulated"].toInt();
	const bool preciseCumulated = json["preciseCumulated"].toBool();
	const bool download = json["download"].toBool();
	const QString &pathOverride = json["pathOverride"].toString();
	const QString &filenameOverride = json["filenameOverride"].toString();
	const bool notify = json["notify"].toBool();
	const bool getBlacklisted = json["getBlacklisted"].toBool();

	QStringList postFilters;
	QJsonArray jsonPostFilters = json["postFilters"].toArray();
	for (auto filter : jsonPostFilters) {
		postFilters.append(filter.toString());
	}

	QString lastState;
	QDateTime lastStateSince;
	int lastStateCount = 0;
	if (json.contains("lastState")) {
		lastState = json["lastState"]["state"].toString();
		lastStateSince = QDateTime::fromString(json["lastState"]["since"].toString(), Qt::ISODate);
		lastStateCount = json["lastState"]["count"].toInt();
	}

	SearchQuery query;
	query.read(json["query"].toObject(), profile);

	return Monitor(sites, interval, lastCheck, download, pathOverride, filenameOverride, cumulated, preciseCumulated, query, postFilters, notify, delay, getBlacklisted, lastState, lastStateSince, lastStateCount);
}


bool operator==(const Monitor &lhs, const Monitor &rhs)
{
	return lhs.sites() == rhs.sites()
		&& lhs.interval() == rhs.interval()
		&& lhs.delay() == rhs.delay()
		&& lhs.lastCheck() == rhs.lastCheck()
		&& lhs.cumulated() == rhs.cumulated()
		&& lhs.preciseCumulated() == rhs.preciseCumulated()
		&& lhs.download() == rhs.download()
		&& lhs.pathOverride() == rhs.pathOverride()
		&& lhs.filenameOverride() == rhs.filenameOverride()
		&& lhs.query() == rhs.query()
		&& lhs.postFilters() == rhs.postFilters()
		&& lhs.notify() == rhs.notify()
		&& lhs.getBlacklisted() == rhs.getBlacklisted();
}

bool operator!=(const Monitor &lhs, const Monitor &rhs)
{ return !(lhs == rhs); }
