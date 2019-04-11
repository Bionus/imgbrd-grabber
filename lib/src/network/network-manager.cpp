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


NetworkReply *NetworkManager::get(QNetworkRequest request)
{
	auto *reply = new NetworkReply(std::move(request), m_manager, this);
	append(reply);

	return reply;
}

NetworkReply *NetworkManager::post(QNetworkRequest request, QByteArray data)
{
	auto *reply = new NetworkReply(std::move(request), std::move(data), m_manager, this);
	append(reply);

	return reply;
}

void NetworkManager::append(NetworkReply *reply)
{
	m_queue.append(reply);

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

	NetworkReply *reply = m_queue.dequeue();
	connect(reply, &NetworkReply::finished, this, &NetworkManager::next);
	reply->start();
}
