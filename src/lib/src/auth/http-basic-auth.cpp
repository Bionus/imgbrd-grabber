#include "auth/http-basic-auth.h"
#include <utility>


HttpBasicAuth::HttpBasicAuth(QString type, int maxPage, QString passwordType, QString tokenType)
	: Auth(std::move(type)), m_maxPage(maxPage), m_passwordType(std::move(passwordType)), m_tokenType(std::move(tokenType))
{
	if (m_tokenType.isEmpty()) {
		m_tokenType = "Basic";
	}
}


int HttpBasicAuth::maxPage() const
{
	return m_maxPage;
}

QString HttpBasicAuth::passwordType() const
{
	return m_passwordType;
}

QString HttpBasicAuth::tokenType() const
{
	return m_tokenType;
}

QList<AuthSettingField> HttpBasicAuth::settingFields() const
{
	AuthSettingField pseudoField;
	pseudoField.id = "pseudo";

	AuthSettingField passwordField;
	passwordField.id = m_passwordType;
	passwordField.isPassword = true;

	return { pseudoField, passwordField };
}
