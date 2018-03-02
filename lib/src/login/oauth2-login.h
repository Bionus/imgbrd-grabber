#ifndef OAUTH2_LOGIN_H
#define OAUTH2_LOGIN_H

#include <QNetworkAccessManager>
#include <QSettings>
#include "login/login.h"


class MixedSettings;
class Site;

class OAuth2Login : public Login
{
	Q_OBJECT

	public:
		explicit OAuth2Login(Site *site, QNetworkAccessManager *manager, MixedSettings *settings);
		bool isTestable() const override;
		void complementRequest(QNetworkRequest *request) const override;

	public slots:
		void login() override;

	protected slots:
		void loginFinished();

	private:
		Site *m_site;
		QNetworkAccessManager *m_manager;
		MixedSettings *m_settings;
		QNetworkReply *m_tokenReply;
		QString m_token;
};

#endif // OAUTH2_LOGIN_H
