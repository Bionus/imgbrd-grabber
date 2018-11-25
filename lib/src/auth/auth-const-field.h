#ifndef AUTH_CONST_FIELD_H
#define AUTH_CONST_FIELD_H

#include <QString>
#include "auth/auth-field.h"


class MixedSettings;

class AuthConstField : public AuthField
{
	public:
		AuthConstField(QString key, QString value);
		QString value(MixedSettings *settings) const override;

	private:
		QString m_value;
};

#endif // AUTH_CONST_FIELD_H
