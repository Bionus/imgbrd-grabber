#include "auth/auth-field.h"
#include <QSettings>


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

QString AuthField::value(QSettings *settings) const
{
	const QString key = type() == FieldType::Username ? "pseudo" : "password";
	return settings->value("auth/" + key).toString();
}
