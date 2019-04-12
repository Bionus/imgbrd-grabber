#ifndef HTTP_POST_LOGIN_H
#define HTTP_POST_LOGIN_H

#include "login/http-login.h"


class HttpAuth;
class MixedSettings;
class NetworkManager;
class NetworkReply;
class QUrlQuery;
class Site;

class HttpPostLogin : public HttpLogin
{
	Q_OBJECT

	public:
		explicit HttpPostLogin(HttpAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings);
		NetworkReply *getReply(const QUrl &url, const QUrlQuery &query) const override;
};

#endif // HTTP_POST_LOGIN_H
