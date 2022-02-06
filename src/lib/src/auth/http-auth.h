#ifndef HTTP_AUTH_H
#define HTTP_AUTH_H

#include <QList>
#include <QString>
#include "auth/field-auth.h"


class AuthField;

class HttpAuth : public FieldAuth
{
	public:
		HttpAuth(QString type, QString url, QList<AuthField*> fields, QString cookie, QString redirectUrl);
		QString url() const;
		QString cookie() const;
		QString redirectUrl() const;

	private:
		QString m_url;
		QString m_cookie;
		QString m_redirectUrl;
};

#endif // HTTP_AUTH_H
