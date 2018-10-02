#ifndef HTTP_POST_LOGIN_H
#define HTTP_POST_LOGIN_H

#include "login/http-login.h"


class CustomNetworkAccessManager;
class HttpAuth;
class MixedSettings;
class QNetworkReply;
class QUrlQuery;
class Site;

class HttpPostLogin : public HttpLogin
{
	Q_OBJECT

	public:
		explicit HttpPostLogin(HttpAuth *auth, Site *site, CustomNetworkAccessManager *manager, MixedSettings *settings);
		QNetworkReply *getReply(const QString &url, const QUrlQuery &query) const override;
};

#endif // HTTP_POST_LOGIN_H
