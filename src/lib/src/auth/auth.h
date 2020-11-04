#ifndef AUTH_H
#define AUTH_H

#include <QList>
#include <QString>


struct AuthSettingField
{
	QString id;
	QString def = "";
	bool isPassword = false;
};

class Auth
{
	public:
		explicit Auth(QString type);
		virtual ~Auth() = default;
		QString type() const;
		virtual QList<AuthSettingField> settingFields() const = 0;

	private:
		QString m_type;
};

#endif // AUTH_H
