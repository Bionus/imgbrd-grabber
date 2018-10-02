#include "auth/http-auth.h"


HttpAuth::HttpAuth(QString type, QString url, QList<AuthField*> fields)
	: FieldAuth(std::move(type), std::move(fields)), m_url(std::move(url))
{}


QString HttpAuth::url() const
{
	return m_url;
}
