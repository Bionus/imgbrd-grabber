#ifndef HTTP_GET_LOGIN_H
#define HTTP_GET_LOGIN_H

#include "login/http-login.h"


class CustomNetworkAccessManager;
class MixedSettings;
class QNetworkReply;
class QUrlQuery;
class Site;

class HttpGetLogin : public HttpLogin
{
	Q_OBJECT

	public:
		explicit HttpGetLogin(Site *site, CustomNetworkAccessManager *manager, MixedSettings *settings);
		QNetworkReply *getReply(const QString &url, const QUrlQuery &query) const override;
};

#endif // HTTP_GET_LOGIN_H
