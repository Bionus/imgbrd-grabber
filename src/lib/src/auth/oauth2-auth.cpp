#include "auth/oauth2-auth.h"
#include <utility>


OAuth2Auth::OAuth2Auth(QString type, QString authType, QString tokenUrl)
	: Auth(std::move(type)), m_authType(std::move(authType)), m_tokenUrl(std::move(tokenUrl))
{}


QString OAuth2Auth::authType() const
{
	return m_authType;
}

QString OAuth2Auth::tokenUrl() const
{
	return m_tokenUrl;
}

QList<AuthSettingField> OAuth2Auth::settingFields() const
{
	AuthSettingField consumerKeyField;
	consumerKeyField.id = "consumerKey";

	AuthSettingField consumerSecretField;
	consumerSecretField.id = "consumerSecret";

	QList<AuthSettingField> fields { consumerKeyField, consumerSecretField };

	if (m_authType == "password") {
		AuthSettingField pseudoField;
		pseudoField.id = "pseudo";

		AuthSettingField passwordField;
		passwordField.id = "password";
		passwordField.isPassword = true;

		fields.append(pseudoField);
		fields.append(passwordField);
	}

	return fields;
}
