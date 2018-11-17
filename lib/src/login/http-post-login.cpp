#include "login/http-post-login.h"
#include <QUrlQuery>
#include "custom-network-access-manager.h"
#include "models/site.h"


HttpPostLogin::HttpPostLogin(Site *site, CustomNetworkAccessManager *manager, MixedSettings *settings)
	: HttpLogin("post", site, manager, settings)
{}

QNetworkReply *HttpPostLogin::getReply(const QString &url, const QUrlQuery &query) const
{
	QNetworkRequest request(m_site->fixUrl(url));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	return m_manager->post(request, query.query(QUrl::FullyEncoded).toUtf8());
}
