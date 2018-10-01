#ifndef AUTH_H
#define AUTH_H

#include <QList>
#include <QPair>
#include <QString>


class AuthField;
class Login;
class MixedSettings;

class Auth
{
    public:
        Auth(QString type, QList<AuthField*> fields);
        QString type() const;
		QList<AuthField*> fields() const;

    private:
		QString m_type;
        QList<AuthField*> m_fields;
};

#endif // AUTH_H
