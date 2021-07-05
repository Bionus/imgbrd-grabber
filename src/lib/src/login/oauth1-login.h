#ifndef OAUTH1_LOGIN_H
#define OAUTH1_LOGIN_H

#include "login/login.h"


class MixedSettings;
class NetworkManager;
class OAuth1Auth;
class QOAuth1Setup;
class Site;

class OAuth1Login : public Login
{
	Q_OBJECT

	public:
		explicit OAuth1Login(OAuth1Auth *auth, Site *site, NetworkManager *manager, MixedSettings *settings);
		bool isTestable() const override;
		void complementRequest(QNetworkRequest *request) const override;

	public slots:
		void login() override;

	private:
		OAuth1Auth *m_auth;
		Site *m_site;
		NetworkManager *m_manager;
		MixedSettings *m_settings;

		QOAuth1Setup *m_oauth1;
};

#endif // OAUTH1_LOGIN_H
