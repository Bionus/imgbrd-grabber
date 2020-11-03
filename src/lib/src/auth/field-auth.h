#ifndef FIELD_AUTH_H
#define FIELD_AUTH_H

#include <QList>
#include "auth/auth.h"


class AuthField;
class QString;

class FieldAuth : public Auth
{
	public:
		FieldAuth(QString type, QList<AuthField*> fields);
		QList<AuthField*> fields() const;
		QList<AuthSettingField> settingFields() const override;

	private:
		QList<AuthField*> m_fields;
};

#endif // FIELD_AUTH_H
