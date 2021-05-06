#include "site-network-access-manager.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include "models/site.h"


SiteNetworkAccessManager::SiteNetworkAccessManager(Site *site, QObject *parent)
	: QNetworkAccessManager(parent), m_site(site)
{}

QNetworkReply *SiteNetworkAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData)
{
	QNetworkRequest newReq(originalReq);
	m_site->setRequestHeaders(newReq);
	return QNetworkAccessManager::createRequest(op, newReq, outgoingData);
}
