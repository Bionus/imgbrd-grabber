#include "auth/oauth2-auth.h"
#include <utility>
#include "js-helpers.h"


OAuth2Auth::OAuth2Auth(QString type, QString authType, QString tokenUrl, QString authorizationUrl, QString redirectUrl, QString urlProtocol, QString clientAuthentication)
	: Auth(std::move(type)), m_authType(std::move(authType)), m_tokenUrl(std::move(tokenUrl)), m_authorizationUrl(std::move(authorizationUrl)), m_redirectUrl(std::move(redirectUrl)), m_urlProtocol(std::move(urlProtocol)), m_clientAuthentication(std::move(clientAuthentication))
{}

OAuth2Auth::OAuth2Auth(QString type, const QJSValue &value)
	: Auth(std::move(type))
{
	getProperty(value, "authType", m_authType);
	getProperty(value, "clientAuthentication", m_clientAuthentication);

	getProperty(value, "tokenUrl", m_tokenUrl);
	getProperty(value, "authorizationUrl", m_authorizationUrl);
	getProperty(value, "redirectUrl", m_redirectUrl);

	getProperty(value, "urlProtocol", m_urlProtocol);
	getProperty(value, "scopes", m_scopes);
}


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

QString OAuth2Auth::clientAuthentication() const
{
	return m_clientAuthentication;
}

QStringList OAuth2Auth::scopes() const
{
	return m_scopes;
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
