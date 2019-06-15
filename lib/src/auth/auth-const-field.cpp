#include "auth/auth-const-field.h"
#include <QtGlobal>
#include <utility>
#include "mixed-settings.h"


AuthConstField::AuthConstField(QString key, QString value)
	: AuthField(QString(), std::move(key), AuthField::Const), m_value(std::move(value))
{}


QString AuthConstField::value(MixedSettings *settings) const
{
	Q_UNUSED(settings);

	return m_value;
}
