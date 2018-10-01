#ifndef AUTH_HASH_FIELD_H
#define AUTH_HASH_FIELD_H

#include <QCryptographicHash>
#include <QString>
#include "auth/auth-field.h"


class AuthHashField : public AuthField
{
    public:
        AuthHashField(QString key, QCryptographicHash::Algorithm algo, QString salt);
        QString value(const QString &val) const override;

    private:
        QCryptographicHash::Algorithm m_algo;
        QString m_salt;
};

#endif // AUTH_HASH_FIELD_H
