#ifndef HTTP_POST_LOGIN_H
#define HTTP_POST_LOGIN_H

#include <QNetworkReply>
#include "login/http-login.h"


class Site;

class HttpPostLogin : public HttpLogin
{
	Q_OBJECT

	public:
		explicit HttpPostLogin(Site *site, CustomNetworkAccessManager *manager, MixedSettings *settings);
		QNetworkReply *getReply(const QString &url, const QUrlQuery &query) const override;
};

#endif // HTTP_POST_LOGIN_H
