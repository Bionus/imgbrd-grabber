#include "auth/field-auth.h"
#include <utility>
#include "auth/auth-field.h"


FieldAuth::FieldAuth(QString type, QList<AuthField*> fields)
	: Auth(std::move(type)), m_fields(std::move(fields))
{}


QList<AuthField*> FieldAuth::fields() const
{
	return m_fields;
}

QList<AuthSettingField> FieldAuth::settingFields() const
{
	QList<AuthSettingField> fields;

	for (AuthField *f : m_fields) {
		const QString fid = f->id();
		if (fid.isEmpty()) {
			continue;
		}

		AuthSettingField field;
		field.id = fid;
		field.def = f->def();
		field.isPassword = f->type() == AuthField::Password;
		fields.append(field);
	}

	return fields;
}
