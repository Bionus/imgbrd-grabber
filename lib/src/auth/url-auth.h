#ifndef URL_AUTH_H
#define URL_AUTH_H

#include <QList>
#include <QString>
#include "auth/auth.h"


class AuthField;

class UrlAuth : public Auth
{
    public:
		UrlAuth(QString type, QList<AuthField*> fields);
		QList<AuthField*> fields() const;

    private:
        QList<AuthField*> m_fields;
};

#endif // URL_AUTH_H
