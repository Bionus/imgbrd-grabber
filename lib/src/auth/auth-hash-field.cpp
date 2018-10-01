#include "auth/auth-hash-field.h"
#include "mixed-settings.h"


AuthHashField::AuthHashField(QString key, QCryptographicHash::Algorithm algo, QString salt)
    : AuthField(std::move(key), AuthField::Hash), m_algo(algo), m_salt(std::move(salt))
{}


QString AuthHashField::value(MixedSettings *settings) const
{
	const QString username = settings->value("auth/pseudo").toString();
	const QString password = settings->value("auth/password").toString();

	QString data = password;
	if (!m_salt.isEmpty())
	{
        data = QString(m_salt);
		data.replace("%username%", username);
		data.replace("%username:lower%", username.toLower());
		data.replace("%password%", password);
    }

	return QCryptographicHash::hash(data.toUtf8(), m_algo).toHex();
}
