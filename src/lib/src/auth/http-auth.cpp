#include "auth/http-auth.h"
#include <utility>


HttpAuth::HttpAuth(QString type, QString url, QList<AuthField*> fields, QString cookie, QString redirectUrl, QString csrfUrl, QStringList csrfFields, QMap<QString, QString> headers)
	: FieldAuth(std::move(type), std::move(fields)), m_url(std::move(url)), m_cookie(std::move(cookie)), m_redirectUrl(std::move(redirectUrl)), m_csrfUrl(std::move(csrfUrl)), m_csrfFields(std::move(csrfFields)), m_headers(std::move(headers))
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

QString HttpAuth::csrfUrl() const
{
	return m_csrfUrl;
}

QStringList HttpAuth::csrfFields() const
{
	return m_csrfFields;
}

const QMap<QString, QString> &HttpAuth::headers() const
{
	return m_headers;
}
