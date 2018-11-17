#include "models/monitor.h"
#include <QMap>
#include "models/site.h"


Monitor::Monitor(Site *site, int interval, QDateTime lastCheck, int cumulated, bool preciseCumulated)
	: m_site(site), m_interval(interval), m_lastCheck(std::move(lastCheck)), m_cumulated(cumulated), m_preciseCumulated(preciseCumulated)
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


void Monitor::toJson(QJsonObject &json) const
{
	json["site"] = m_site->url();
	json["interval"] = m_interval;
	json["lastCheck"] = m_lastCheck.toString(Qt::ISODate);
	json["cumulated"] = m_cumulated;
	json["preciseCumulated"] = m_preciseCumulated;
}

Monitor Monitor::fromJson(const QJsonObject &json, const QMap<QString, Site*> &sites)
{
	Site *site = sites.value(json["site"].toString());
	const int interval = json["interval"].toInt();
	const QDateTime lastCheck = QDateTime::fromString(json["lastCheck"].toString(), Qt::ISODate);
	const int cumulated = json["cumulated"].toInt();
	const bool preciseCumulated = json["preciseCumulated"].toBool();
	return Monitor(site, interval, lastCheck, cumulated, preciseCumulated);
}


bool operator==(const Monitor &lhs, const Monitor &rhs)
{
	return lhs.site() == rhs.site()
		&& lhs.interval() == rhs.interval()
		&& lhs.lastCheck() == rhs.lastCheck()
		&& lhs.cumulated() == rhs.cumulated()
		&& lhs.preciseCumulated() == rhs.preciseCumulated();
}

bool operator!=(const Monitor &lhs, const Monitor &rhs)
{ return !(lhs == rhs); }
