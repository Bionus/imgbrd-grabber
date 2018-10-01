#include "auth/auth.h"
#include "auth/auth-field.h"


Auth::Auth(QString type, QList<AuthField*> fields)
    : m_type(std::move(type)), m_fields(std::move(fields))
{}


QString Auth::type() const
{
    return m_type;
}

QList<AuthField*> Auth::fields() const
{
    return m_fields;
}
