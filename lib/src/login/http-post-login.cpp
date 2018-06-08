#include "login/http-post-login.h"
#include <QUrlQuery>
#include "custom-network-access-manager.h"
#include "models/site.h"


HttpPostLogin::HttpPostLogin(Site *site, CustomNetworkAccessManager *manager, MixedSettings *settings)
	: HttpLogin("post", site, manager, settings)
{}

QNetworkReply *HttpPostLogin::getReply(const QString &loginUrl, const QUrlQuery &query) const
{
	QNetworkRequest request(m_site->fixUrl(loginUrl));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	return m_manager->post(request, query.query(QUrl::FullyEncoded).toUtf8());
}
