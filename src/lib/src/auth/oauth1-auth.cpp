#include "auth/oauth1-auth.h"
#include <utility>
#include "js-helpers.h"


OAuth1Auth::OAuth1Auth(QString type, QString temporaryCredentialsUrl, QString authorizationUrl, QString tokenCredentialsUrl)
	: Auth(std::move(type)), m_temporaryCredentialsUrl(std::move(temporaryCredentialsUrl)), m_authorizationUrl(std::move(authorizationUrl)), m_tokenCredentialsUrl(std::move(tokenCredentialsUrl))
{}

OAuth1Auth::OAuth1Auth(QString type, const QJSValue &value)
	: Auth(std::move(type))
{
	getProperty(value, "temporaryCredentialsUrl", m_temporaryCredentialsUrl);
	getProperty(value, "authorizationUrl", m_authorizationUrl);
	getProperty(value, "tokenCredentialsUrl", m_tokenCredentialsUrl);
	getProperty(value, "signatureMethod", m_signatureMethod);
}


QString OAuth1Auth::temporaryCredentialsUrl() const
{
	return m_temporaryCredentialsUrl;
}

QString OAuth1Auth::authorizationUrl() const
{
	return m_authorizationUrl;
}

QString OAuth1Auth::tokenCredentialsUrl() const
{
	return m_tokenCredentialsUrl;
}

QString OAuth1Auth::signatureMethod() const
{
	return m_signatureMethod;
}

QList<AuthSettingField> OAuth1Auth::settingFields() const
{
	AuthSettingField consumerKeyField;
	consumerKeyField.id = "consumerKey";

	AuthSettingField consumerSecretField;
	consumerSecretField.id = "consumerSecret";

	return QList<AuthSettingField> { consumerKeyField, consumerSecretField };
}
