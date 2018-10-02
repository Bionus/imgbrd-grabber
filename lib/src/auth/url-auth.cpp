#include "auth/url-auth.h"


UrlAuth::UrlAuth(QString type, QList<AuthField*> fields)
    : Auth(std::move(type)), m_fields(std::move(fields))
{}


QList<AuthField*> UrlAuth::fields() const
{
    return m_fields;
}
