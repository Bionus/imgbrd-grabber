#ifndef OAUTH2_AUTH_H
#define OAUTH2_AUTH_H

#include <QString>
#include "auth/auth.h"


class OAuth2Auth : public Auth
{
	public:
		OAuth2Auth(QString type, QString authType, QString tokenUrl, QString authorizationUrl, QString redirectUrl, QString urlProtocol, QString clientAuthentication = {});
		QString authType() const;
		QString tokenUrl() const;
		QString authorizationUrl() const;
		QString redirectUrl() const;
		QString urlProtocol() const;
		QString clientAuthentication() const;
		QString name() const override;
		QList<AuthSettingField> settingFields() const override;

	private:
		QString m_authType;
		QString m_tokenUrl;
		QString m_authorizationUrl;
		QString m_redirectUrl;
		QString m_urlProtocol;
		QString m_clientAuthentication;
};

#endif // OAUTH2_AUTH_H
