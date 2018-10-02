#include "auth/url-auth.h"


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
