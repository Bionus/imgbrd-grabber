#include "auth/auth-hash-field.h"
#include <QByteArray>
#include <QVariant>
#include <utility>
#include "mixed-settings.h"


AuthHashField::AuthHashField(QString key, QCryptographicHash::Algorithm algorithm, QString salt)
	: AuthField(QString(), std::move(key), AuthField::Hash), m_algorithm(algorithm), m_salt(std::move(salt))
{}


QString AuthHashField::value(MixedSettings *settings) const
{
	const QString username = settings->value("auth/pseudo").toString();
	const QString password = settings->value("auth/password").toString();

	// Don't hash passwords twice
	// FIXME: very long passwords won't get hashed
	if (password.length() >= 32) {
		return password;
	}

	QString data = password;
	if (!m_salt.isEmpty() && (!username.isEmpty() || !password.isEmpty())) {
		data = QString(m_salt);
		data.replace("%pseudo%", username);
		data.replace("%pseudo:lower%", username.toLower());
		data.replace("%password%", password);
	}

	// Don't hash empty strings
	if (data.isEmpty()) {
		return data;
	}

	return QCryptographicHash::hash(data.toUtf8(), m_algorithm).toHex();
}

QString AuthHashField::salt() const
{
	return m_salt;
}
