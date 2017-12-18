#ifndef HTTP_LOGIN_H
#define HTTP_LOGIN_H

#include <QNetworkReply>
#include <QSettings>
#include "login/login.h"
#include "mixed-settings.h"


class Site;

class HttpLogin : public Login
{
	Q_OBJECT

	public:
		explicit HttpLogin(const QString &type, Site *site, QNetworkAccessManager *manager, QNetworkCookieJar *cookieJar, MixedSettings *settings);
		bool isTestable() const override;
		virtual QNetworkReply *getReply(const QString &url, const QUrlQuery &query) const = 0;

	public slots:
		void login() override;

	protected slots:
		void loginFinished();

	protected:
		QString m_type;
		Site *m_site;
		QNetworkReply *m_loginReply;
		QNetworkAccessManager *m_manager;
		QNetworkCookieJar *m_cookieJar;
		MixedSettings *m_settings;
};

#endif // HTTP_LOGIN_H
