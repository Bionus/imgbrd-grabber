#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QAtomicInt>
#include <QObject>
#include <QPair>
#include <QQueue>
#include "throttling-manager.h"


class CustomNetworkAccessManager;
class NetworkReply;
class QAbstractNetworkCache;
class QByteArray;
class QNetworkCookieJar;
class QNetworkRequest;

class NetworkManager : public QObject
{
	Q_OBJECT

	public:
		explicit NetworkManager(QObject *parent = nullptr);

		int maxConcurrency() const;
		void setMaxConcurrency(int maxConcurrency);
		int interval(int key) const;
		void setInterval(int key, int msInterval);

		void setCache(QAbstractNetworkCache *cache);
		QNetworkCookieJar *cookieJar() const;
		void setCookieJar(QNetworkCookieJar *cookieJar);

		NetworkReply *get(QNetworkRequest request, int type = -1);
		NetworkReply *post(QNetworkRequest request, QByteArray data, int type = -1);
		void clear();

	protected:
		void append(NetworkReply *reply, int type = -1);

	protected slots:
		void next();

	private:
		CustomNetworkAccessManager *m_manager;
		ThrottlingManager m_throttlingManager;
		int m_maxConcurrency = 6;
		QQueue<QPair<int, NetworkReply*>> m_queue;
		QAtomicInt m_activeQueries;
};

#endif // NETWORK_MANAGER_H
