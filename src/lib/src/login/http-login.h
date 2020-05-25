#ifndef HTTP_LOGIN_H
#define HTTP_LOGIN_H

#include <QString>
#include "login/login.h"


class HttpAuth;
class MixedSettings;
class NetworkManager;
class NetworkReply;
class QUrl;
class QUrlQuery;
class Site;

class HttpLogin : public Login
{
	Q_OBJECT

	protected:
		HttpLogin(QString type, HttpAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings);

	public:
		virtual ~HttpLogin() = default;
		bool isTestable() const override;
		virtual NetworkReply *getReply(const QUrl &url, const QUrlQuery &query) const = 0;

	private:
		bool hasCookie(const QUrl &url) const;

	public slots:
		void login() override;

	protected slots:
		void loginFinished();

	protected:
		QString m_type;
		HttpAuth *m_auth;
		Site *m_site;
		NetworkReply *m_loginReply;
		NetworkManager *m_manager;
		MixedSettings *m_settings;
};

#endif // HTTP_LOGIN_H
