#include "auth/auth-const-field.h"
#include "mixed-settings.h"


AuthConstField::AuthConstField(QString key, QString value)
	: AuthField(std::move(key), AuthField::Const), m_value(std::move(value))
{}


QString AuthConstField::value(MixedSettings *settings) const
{
	Q_UNUSED(settings);

	return m_value;
}
