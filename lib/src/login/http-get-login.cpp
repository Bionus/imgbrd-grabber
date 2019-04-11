#include "login/http-get-login.h"
#include <QUrlQuery>
#include "models/site.h"
#include "network/network-manager.h"


HttpGetLogin::HttpGetLogin(HttpAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings)
	: HttpLogin("get", auth, site, manager, settings)
{}

NetworkReply *HttpGetLogin::getReply(const QString &url, const QUrlQuery &query) const
{
	QUrl fixedUrl = m_site->fixUrl(url);
	fixedUrl.setQuery(query);
	const QNetworkRequest request(fixedUrl);

	return m_manager->get(request);
}
