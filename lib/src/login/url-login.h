#ifndef URL_LOGIN_H
#define URL_LOGIN_H

#include <QString>
#include "login/login.h"


class AuthField;
class MixedSettings;
class Page;
class QNetworkAccessManager;
class Site;
class UrlAuth;

class UrlLogin : public Login
{
	Q_OBJECT

	public:
		explicit UrlLogin(UrlAuth *auth, Site *site, QNetworkAccessManager *manager, MixedSettings *settings);
		bool isTestable() const override;
		QString complementUrl(QString url) const override;

	public slots:
		void login() override;

	protected slots:
		void loginFinished();

	private:
		UrlAuth *m_auth;
		Site *m_site;
		QNetworkAccessManager *m_manager;
		MixedSettings *m_settings;
		Page *m_page;
};

#endif // URL_LOGIN_H
