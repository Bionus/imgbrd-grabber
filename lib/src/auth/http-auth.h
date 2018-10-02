#ifndef HTTP_AUTH_H
#define HTTP_AUTH_H

#include <QList>
#include <QString>
#include "auth/auth.h"


class AuthField;

class HttpAuth : public Auth
{
    public:
		HttpAuth(QString type, QString url, QList<AuthField*> fields);
        QString url() const;
		QList<AuthField*> fields() const;

    private:
        QString m_url;
        QList<AuthField*> m_fields;
};

#endif // HTTP_AUTH_H
