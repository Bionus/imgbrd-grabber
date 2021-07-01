#include "auth/oauth2-auth.h"
#include <utility>


OAuth2Auth::OAuth2Auth(QString type, QString authType, QString tokenUrl, QString authorizationUrl, QString redirectUrl, QString urlProtocol)
	: Auth(std::move(type)), m_authType(std::move(authType)), m_tokenUrl(std::move(tokenUrl)), m_authorizationUrl(std::move(authorizationUrl)), m_redirectUrl(std::move(redirectUrl)), m_urlProtocol(std::move(urlProtocol))
{}


QString OAuth2Auth::authType() const
{
	return m_authType;
}

QString OAuth2Auth::tokenUrl() const
{
	return m_tokenUrl;
}

QString OAuth2Auth::authorizationUrl() const
{
	return m_authorizationUrl;
}

QString OAuth2Auth::redirectUrl() const
{
	return m_redirectUrl;
}

QString OAuth2Auth::urlProtocol() const
{
	return m_urlProtocol;
}

QString OAuth2Auth::name() const
{
	return type() + "_" + m_authType;
}

QList<AuthSettingField> OAuth2Auth::settingFields() const
{
	AuthSettingField consumerKeyField;
	consumerKeyField.id = "consumerKey";

	AuthSettingField consumerSecretField;
	consumerSecretField.id = "consumerSecret";

	QList<AuthSettingField> fields { consumerKeyField, consumerSecretField };

	if (m_authType == "password" || m_authType == "password_json") {
		AuthSettingField pseudoField;
		pseudoField.id = "pseudo";

		AuthSettingField passwordField;
		passwordField.id = "password";
		passwordField.isPassword = true;

		fields.append(pseudoField);
		fields.append(passwordField);
	}

	if (m_authType == "refresh_token") {
		AuthSettingField accessTokenField;
		accessTokenField.id = "accessToken";

		AuthSettingField refreshTokenField;
		refreshTokenField.id = "refreshToken";

		fields.append(accessTokenField);
		fields.append(refreshTokenField);
	}

	return fields;
}
