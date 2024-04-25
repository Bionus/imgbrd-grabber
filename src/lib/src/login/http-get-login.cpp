#include "login/http-get-login.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include "auth/http-auth.h"
#include "models/site.h"
#include "network/network-manager.h"


HttpGetLogin::HttpGetLogin(HttpAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings)
	: HttpLogin("get", auth, site, manager, settings)
{}

NetworkReply *HttpGetLogin::getReply(const QUrl &url, const QUrlQuery &query) const
{
	QUrl fixedUrl = url;
	fixedUrl.setQuery(query);
	QNetworkRequest request(fixedUrl);

	for (auto it = m_auth->headers().constBegin(); it != m_auth->headers().constEnd(); ++it) {
		request.setRawHeader(it.key().toLatin1(), it.value().toLatin1());
	}

	return m_manager->get(request);
}
