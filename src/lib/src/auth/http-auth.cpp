#include "auth/http-auth.h"
#include <utility>


HttpAuth::HttpAuth(QString type, QString url, QList<AuthField*> fields, QString cookie, QString redirectUrl)
	: FieldAuth(std::move(type), std::move(fields)), m_url(std::move(url)), m_cookie(std::move(cookie)), m_redirectUrl(std::move(redirectUrl))
{}


QString HttpAuth::url() const
{
	return m_url;
}

QString HttpAuth::cookie() const
{
	return m_cookie;
}

QString HttpAuth::redirectUrl() const
{
	return m_redirectUrl;
}
