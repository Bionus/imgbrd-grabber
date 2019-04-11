#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QAtomicInt>
#include <QByteArray>
#include <QNetworkRequest>
#include <QObject>
#include <QQueue>


class CustomNetworkAccessManager;
class NetworkReply;
class QAbstractNetworkCache;
class QNetworkCookieJar;

class NetworkManager : public QObject
{
	Q_OBJECT

	public:
		explicit NetworkManager(QObject *parent = nullptr);
		int maxConcurrency() const;
		void setMaxConcurrency(int maxConcurrency);
		void setCache(QAbstractNetworkCache *cache);
		QNetworkCookieJar *cookieJar() const;
		void setCookieJar(QNetworkCookieJar *cookieJar);
		NetworkReply *get(QNetworkRequest request);
		NetworkReply *post(QNetworkRequest request, QByteArray data);

	protected:
		void append(NetworkReply *reply);

	protected slots:
		void next();

	private:
		CustomNetworkAccessManager *m_manager;
		int m_maxConcurrency = 6;
		QQueue<NetworkReply*> m_queue;
		QAtomicInt m_activeQueries;
};

#endif // NETWORK_MANAGER_H
