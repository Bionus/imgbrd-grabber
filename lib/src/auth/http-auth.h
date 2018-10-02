#ifndef HTTP_AUTH_H
#define HTTP_AUTH_H

#include <QList>
#include <QString>
#include "auth/field-auth.h"


class AuthField;

class HttpAuth : public FieldAuth
{
	public:
		HttpAuth(QString type, QString url, QList<AuthField*> fields);
		QString url() const;

	private:
		QString m_url;
};

#endif // HTTP_AUTH_H
