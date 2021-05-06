#ifndef SITE_NETWORK_ACCESS_MANAGER_H
#define SITE_NETWORK_ACCESS_MANAGER_H

#include <QNetworkAccessManager>


class Site;

class SiteNetworkAccessManager : public QNetworkAccessManager
{
	public:
		explicit SiteNetworkAccessManager(Site *site, QObject *parent = nullptr);

	protected:
		QNetworkReply *createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr) override;

    private:
        Site *m_site;
};

#endif // SITE_NETWORK_ACCESS_MANAGER_H
