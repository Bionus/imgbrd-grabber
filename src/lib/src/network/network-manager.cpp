#include "network-manager.h"
#include <utility>
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

int NetworkManager::activeQueries() const
{
	return m_activeQueries;
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

void NetworkManager::clear()
{
	m_queue.clear();
}


void NetworkManager::next()
{
	// Loop rather than recurse: a queue full of dead or aborted replies used to
	// recurse once per entry, which overflows the stack on a large batch.
	forever {
		if (m_queue.isEmpty()) {
			m_activeQueries.fetchAndAddRelaxed(-1);
			return;
		}

		auto pair = m_queue.dequeue();
		int type = pair.first;
		QPointer<NetworkReply> reply = pair.second;

		if (!reply.isNull() && reply->isRunning()) {
			// Release the slot on whichever comes first, and only once. Waiting on
			// finished() alone leaks a slot forever when a reply is aborted or deleted
			// before it completes, and a leaked slot per abort eventually wedges every
			// download until the app is restarted.
			auto released = std::make_shared<bool>(false);
			auto release = [this, released]() {
				if (*released) {
					return;
				}
				*released = true;
				next();
			};
			connect(reply, &NetworkReply::finished, this, release);
			connect(reply, &QObject::destroyed, this, release);

			m_throttlingManager.start(type, reply);
			return;
		}
	}
}
