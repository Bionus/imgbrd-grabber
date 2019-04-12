#ifndef OAUTH2_LOGIN_H
#define OAUTH2_LOGIN_H

#include <QDateTime>
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
		void refreshLoginFinished();
		void refreshFinished();

	protected:
		void refresh(bool login = false);
		bool readResponse(NetworkReply *reply);

	private:
		OAuth2Auth *m_auth;
		Site *m_site;
		NetworkManager *m_manager;
		MixedSettings *m_settings;
		NetworkReply *m_tokenReply = nullptr;
		NetworkReply *m_refreshReply = nullptr;
		QString m_accessToken;
		QString m_refreshToken;
		QDateTime m_expires;
};

#endif // OAUTH2_LOGIN_H
