#ifndef HTTP_GET_LOGIN_H
#define HTTP_GET_LOGIN_H

#include "login/http-login.h"


class HttpAuth;
class MixedSettings;
class NetworkManager;
class NetworkReply;
class QUrlQuery;
class Site;

class HttpGetLogin : public HttpLogin
{
	Q_OBJECT

	public:
		explicit HttpGetLogin(HttpAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings);
		NetworkReply *getReply(const QUrl &url, const QUrlQuery &query) const override;
};

#endif // HTTP_GET_LOGIN_H
