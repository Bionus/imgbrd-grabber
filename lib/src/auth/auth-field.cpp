#include "auth/auth-field.h"


AuthField::AuthField(QString key, FieldType type)
    : m_key(std::move(key)), m_type(type)
{}


QString AuthField::key() const
{
    return m_key;
}

AuthField::FieldType AuthField::type() const
{
    return m_type;
}

QString AuthField::value(const QString &val) const
{
    return val;
}
