#include "models/monitor.h"
#include "models/site.h"


Monitor::Monitor(Site *site, int interval, const QDateTime &lastCheck)
	: m_site(site), m_interval(interval), m_lastCheck(lastCheck)
{}

int Monitor::secsToNextCheck() const
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


void Monitor::toJson(QJsonObject &json) const
{
	json["site"] = m_site->url();
	json["interval"] = m_interval;
	json["lastCheck"] = m_lastCheck.toString(Qt::ISODate);
}

Monitor Monitor::fromJson(const QJsonObject &json, const QMap<QString, Site*> &sites)
{
	Site *site = sites.value(json["site"].toString());
	int interval = json["interval"].toInt();
	QDateTime lastCheck = QDateTime::fromString(json["lastCheck"].toString(), Qt::ISODate);

	return Monitor(site, interval, lastCheck);
}
