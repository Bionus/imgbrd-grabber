#ifndef OAUTH2_LOGIN_H
#define OAUTH2_LOGIN_H

#include <QString>
#include "login/login.h"


class MixedSettings;
class NetworkManager;
class NetworkReply;
class OAuth2Auth;
class QNetworkRequest;
class Site;

class OAuth2Login : public Login
{
	Q_OBJECT

	public:
		explicit OAuth2Login(OAuth2Auth *auth, Site *site, NetworkManager *manager, MixedSettings *settings);
		bool isTestable() const override;
		void complementRequest(QNetworkRequest *request) const override;

	public slots:
		void login() override;

	protected slots:
		void loginFinished();

	private:
		OAuth2Auth *m_auth;
		Site *m_site;
		NetworkManager *m_manager;
		MixedSettings *m_settings;
		NetworkReply *m_tokenReply;
		QString m_token;
};

#endif // OAUTH2_LOGIN_H
