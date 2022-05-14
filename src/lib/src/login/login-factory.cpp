#include "login-factory.h"
#include <QString>
#include "auth/http-auth.h"
#include "auth/http-basic-auth.h"
#include "auth/oauth1-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"
#include "logger.h"
#include "login/http-basic-login.h"
#include "login/http-get-login.h"
#include "login/http-post-login.h"
#include "login/oauth1-login.h"
#include "login/oauth2-login.h"
#include "login/url-login.h"
#include "mixed-settings.h"
#include "models/site.h"


Login *LoginFactory::build(Site *site, Auth *auth, NetworkManager *manager)
{
	if (auth == nullptr) {
		return nullptr;
	}

	const QString type = auth->type();
	MixedSettings *settings = site->settings();

	if (type == "url") {
		return new UrlLogin(dynamic_cast<UrlAuth*>(auth), site, manager, settings);
	}
	if (type == "oauth2") {
		return new OAuth2Login(dynamic_cast<OAuth2Auth*>(auth), site, manager, settings);
	}
	if (type == "oauth1") {
		return new OAuth1Login(dynamic_cast<OAuth1Auth*>(auth), site, manager, settings);
	}
	if (type == "post") {
		return new HttpPostLogin(dynamic_cast<HttpAuth*>(auth), site, manager, settings);
	}
	if (type == "get") {
		return new HttpGetLogin(dynamic_cast<HttpAuth*>(auth), site, manager, settings);
	}
	if (type == "http_basic") {
		return new HttpBasicLogin(dynamic_cast<HttpBasicAuth*>(auth), site, manager, settings);
	}

	log(QStringLiteral("[%1] Invalid login type '%1'").arg(site->url(), type), Logger::Error);
	return nullptr;
}
