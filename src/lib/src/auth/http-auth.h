#ifndef HTTP_AUTH_H
#define HTTP_AUTH_H

#include <QList>
#include <QMap>
#include <QString>
#include "auth/field-auth.h"


class AuthField;

class HttpAuth : public FieldAuth
{
	public:
		HttpAuth(QString type, QString url, QList<AuthField*> fields, QString cookie, QString redirectUrl, QString csrfUrl, QStringList csrfFields, QMap<QString, QString> headers);
		QString url() const;
		QString cookie() const;
		QString redirectUrl() const;
		QString csrfUrl() const;
		QStringList csrfFields() const;
		const QMap<QString, QString> &headers() const;

	private:
		QString m_url;
		QString m_cookie;
		QString m_redirectUrl;
		QString m_csrfUrl;
		QStringList m_csrfFields;
		QMap<QString, QString> m_headers;
};

#endif // HTTP_AUTH_H
