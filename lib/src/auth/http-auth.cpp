#include "auth/http-auth.h"


HttpAuth::HttpAuth(QString type, QString url, QList<AuthField*> fields)
    : Auth(std::move(type)), m_url(std::move(url)), m_fields(std::move(fields))
{}


QString HttpAuth::url() const
{
    return m_url;
}

QList<AuthField*> Auth::fields() const
{
    return m_fields;
}
