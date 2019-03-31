#ifndef OAUTH2_LOGIN_H
#define OAUTH2_LOGIN_H

#include <QString>
#include "login/login.h"


class CustomNetworkAccessManager;
class MixedSettings;
class OAuth2Auth;
class QNetworkReply;
class QNetworkRequest;
class Site;

class OAuth2Login : public Login
{
	Q_OBJECT

	public:
		explicit OAuth2Login(OAuth2Auth *auth, Site *site, CustomNetworkAccessManager *manager, MixedSettings *settings);
		bool isTestable() const override;
		void complementRequest(QNetworkRequest *request) const override;

	public slots:
		void login() override;

	protected slots:
		void loginFinished();

	private:
		OAuth2Auth *m_auth;
		Site *m_site;
		CustomNetworkAccessManager *m_manager;
		MixedSettings *m_settings;
		QNetworkReply *m_tokenReply;
		QString m_token;
};

#endif // OAUTH2_LOGIN_H
