#include "auth/auth-field.h"
#include <QString>
#include <QVariant>
#include <utility>
#include "mixed-settings.h"


AuthField::AuthField(QString id, QString key, FieldType type)
	: m_id(std::move(id)), m_key(std::move(key)), m_type(type)
{}


QString AuthField::id() const
{
	return m_id;
}

QString AuthField::key() const
{
	return m_key;
}

AuthField::FieldType AuthField::type() const
{
	return m_type;
}


QString AuthField::value(MixedSettings *settings) const
{
	return settings->value("auth/" + m_id).toString();
}
