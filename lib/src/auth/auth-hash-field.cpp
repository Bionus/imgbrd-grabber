#include "auth/auth-hash-field.h"


AuthHashField::AuthHashField(QString key, QCryptographicHash::Algorithm algo, QString salt)
    : AuthField(std::move(key), AuthField::Hash), m_algo(algo), m_salt(std::move(salt))
{}


QString AuthHashField::value(const QString &val) const
{
    QString data = val;

    if (!m_salt.isEmpty())
    {
        data = QString(m_salt);
        /*data.replace("%username%", pseudo);
        data.replace("%username:lower%", pseudo.toLower());
        data.replace("%password%", password);*/
    }

	return QCryptographicHash::hash(data.toUtf8(), m_algo).toHex();
}
