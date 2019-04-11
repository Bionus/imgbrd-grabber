#include "throttling-manager.h"
#include <QTimer>
#include "network-reply.h"


int ThrottlingManager::interval(int key) const
{
	return m_intervals[key];
}

void ThrottlingManager::setInterval(int key, int msInterval)
{
	m_intervals[key] = msInterval;
}

void ThrottlingManager::clear()
{
	m_intervals.clear();
}


int ThrottlingManager::msToRequest(int key) const
{
	if (!m_lastRequests.contains(key) || !m_lastRequests[key].isValid()) {
		return 0;
	}

	const qint64 sinceLastRequest = m_lastRequests[key].msecsTo(QDateTime::currentDateTime());
	return qMax(0, interval(key) - static_cast<int>(sinceLastRequest));
}

void ThrottlingManager::start(int key, NetworkReply *reply)
{
	int msWait = msToRequest(key);
	m_lastRequests[key] = QDateTime::currentDateTime().addMSecs(msWait);
	reply->start(msWait);
}
