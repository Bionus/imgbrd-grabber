#include "auth/http-basic-auth.h"
#include <utility>


HttpBasicAuth::HttpBasicAuth(QString type, int maxPage, QString passwordType)
	: Auth(std::move(type)), m_maxPage(maxPage), m_passwordType(std::move(passwordType))
{}


int HttpBasicAuth::maxPage() const
{
	return m_maxPage;
}

QString HttpBasicAuth::passwordType() const
{
	return m_passwordType;
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
