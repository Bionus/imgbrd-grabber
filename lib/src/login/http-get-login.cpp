#include "login/http-get-login.h"
#include <QUrlQuery>
#include "custom-network-access-manager.h"
#include "models/site.h"


HttpGetLogin::HttpGetLogin(HttpAuth *auth, Site *site, CustomNetworkAccessManager *manager, MixedSettings *settings)
	: HttpLogin("get", auth, site, manager, settings)
{}

QNetworkReply *HttpGetLogin::getReply(const QString &url, const QUrlQuery &query) const
{
	QUrl fixedUrl = m_site->fixUrl(url);
	fixedUrl.setQuery(query);
	const QNetworkRequest request(fixedUrl);

	return m_manager->get(request);
}
