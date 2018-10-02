#ifndef FIELD_AUTH_H
#define FIELD_AUTH_H

#include <QList>
#include <QString>
#include "auth/auth.h"


class AuthField;

class FieldAuth : public Auth
{
	public:
		FieldAuth(QString type, QList<AuthField*> fields);
		QList<AuthField*> fields() const;

	private:
		QList<AuthField*> m_fields;
};

#endif // FIELD_AUTH_H
