#include "login/http-post-login.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include "models/site.h"
#include "network/network-manager.h"


HttpPostLogin::HttpPostLogin(HttpAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings)
	: HttpLogin("post", auth, site, manager, settings)
{}

NetworkReply *HttpPostLogin::getReply(const QUrl &url, const QUrlQuery &query) const
{
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	QString data = query.query(QUrl::FullyEncoded)
		.replace('+', "%2B")
		.replace("%20", "+");
	return m_manager->post(request, data.toUtf8());
}
