#ifndef URL_LOGIN_H
#define URL_LOGIN_H

#include <QString>
#include "login/login.h"


class MixedSettings;
class NetworkManager;
class Page;
class Site;
class UrlAuth;

class UrlLogin : public Login
{
	Q_OBJECT

	public:
		explicit UrlLogin(UrlAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings);
		bool isTestable() const override;
		QString complementUrl(QString url) const override;

	public slots:
		void login() override;

	protected slots:
		void loginFinished();

	private:
		UrlAuth *m_auth;
		Site *m_site;
		NetworkManager *m_manager;
		MixedSettings *m_settings;
		Page *m_page;
};

#endif // URL_LOGIN_H
