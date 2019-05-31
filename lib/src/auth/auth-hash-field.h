#ifndef AUTH_HASH_FIELD_H
#define AUTH_HASH_FIELD_H

#include <QCryptographicHash>
#include <QString>
#include "auth/auth-field.h"


class MixedSettings;

class AuthHashField : public AuthField
{
	public:
		AuthHashField(QString key, QCryptographicHash::Algorithm algorithm, QString salt);
		QString value(MixedSettings *settings) const override;
		QString salt() const;

	private:
		QCryptographicHash::Algorithm m_algorithm;
		QString m_salt;
};

#endif // AUTH_HASH_FIELD_H
