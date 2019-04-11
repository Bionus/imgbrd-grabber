#include "network-manager.h"
#include "custom-network-access-manager.h"
#include "network-reply.h"


NetworkManager::NetworkManager(QObject *parent)
	: QObject(parent)
{
	m_manager = new CustomNetworkAccessManager(this);
}


int NetworkManager::maxConcurrency() const
{
	return m_maxConcurrency;
}

void NetworkManager::setMaxConcurrency(int maxConcurrency)
{
	m_maxConcurrency = maxConcurrency;
}

int NetworkManager::interval(int key) const
{
	return m_throttlingManager.interval(key);
}

void NetworkManager::setInterval(int key, int msInterval)
{
	m_throttlingManager.setInterval(key, msInterval);
}


void NetworkManager::setCache(QAbstractNetworkCache *cache)
{
	return m_manager->setCache(cache);
}

QNetworkCookieJar *NetworkManager::cookieJar() const
{
	return m_manager->cookieJar();
}

void NetworkManager::setCookieJar(QNetworkCookieJar *cookieJar)
{
	return m_manager->setCookieJar(cookieJar);
}


NetworkReply *NetworkManager::get(QNetworkRequest request, int type)
{
	auto *reply = new NetworkReply(std::move(request), m_manager, this);
	append(reply, type);

	return reply;
}

NetworkReply *NetworkManager::post(QNetworkRequest request, QByteArray data, int type)
{
	auto *reply = new NetworkReply(std::move(request), std::move(data), m_manager, this);
	append(reply, type);

	return reply;
}

void NetworkManager::append(NetworkReply *reply, int type)
{
	m_queue.append({ type, reply });

	int activeQueries = m_activeQueries.fetchAndStoreRelaxed(m_maxConcurrency);
	for (int i = activeQueries; i < m_maxConcurrency; ++i) {
		QTimer::singleShot(0, this, SLOT(next()));
	}
}


void NetworkManager::next()
{
	if (m_queue.isEmpty()) {
		m_activeQueries.fetchAndAddRelaxed(-1);
		return;
	}

	auto pair = m_queue.dequeue();
	int type = pair.first;
	NetworkReply *reply = pair.second;
	connect(reply, &NetworkReply::finished, this, &NetworkManager::next);

	m_throttlingManager.start(type, reply);
}
