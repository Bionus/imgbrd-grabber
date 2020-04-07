#ifndef HTTP_BASIC_LOGIN_H
#define HTTP_BASIC_LOGIN_H

#include "login/login.h"


class HttpBasicAuth;
class MixedSettings;
class NetworkManager;
class Page;
class QNetworkRequest;
class Site;

class HttpBasicLogin : public Login
{
	Q_OBJECT

	public:
		explicit HttpBasicLogin(HttpBasicAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings);
		bool isTestable() const override;
		void complementRequest(QNetworkRequest *request) const override;

	public slots:
		void login() override;

	protected slots:
		void loginFinished();

	private:
		HttpBasicAuth *m_auth;
		Site *m_site;
		NetworkManager *m_manager;
		MixedSettings *m_settings;
		Page *m_page;
};

#endif // HTTP_BASIC_LOGIN_H
