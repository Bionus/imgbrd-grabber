#include "login/http-post-login.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include "auth/http-auth.h"
#include "models/site.h"
#include "network/network-manager.h"


HttpPostLogin::HttpPostLogin(HttpAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings)
	: HttpLogin("post", auth, site, manager, settings)
{}

NetworkReply *HttpPostLogin::getReply(const QUrl &url, const QUrlQuery &query) const
{
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	for (auto it = m_auth->headers().constBegin(); it != m_auth->headers().constEnd(); ++it) {
		request.setRawHeader(it.key().toLatin1(), it.value().toLatin1());
	}

	QString data = query.query(QUrl::FullyEncoded)
		.replace('+', "%2B")
		.replace("%20", "+");
	return m_manager->post(request, data.toUtf8());
}
