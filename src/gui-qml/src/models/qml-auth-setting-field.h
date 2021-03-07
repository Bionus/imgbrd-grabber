#ifndef QML_AUTH_SETTING_FIELD_H
#define QML_AUTH_SETTING_FIELD_H

#include <QObject>
#include <QString>
#include "auth/auth.h"


class QmlAuthSettingField : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString id READ id CONSTANT)
	Q_PROPERTY(QString def READ def CONSTANT)
	Q_PROPERTY(bool isPassword READ isPassword CONSTANT)

	public:
		explicit QmlAuthSettingField(const AuthSettingField &field, QObject *parent = nullptr)
			: QObject(parent), m_id(field.id), m_def(field.def), m_isPassword(field.isPassword) {}

		QString id() const { return m_id; }
		QString def() const { return m_def; }
		bool isPassword() const { return m_isPassword; }

	private:
		QString m_id;
		QString m_def;
		bool m_isPassword;
};

#endif // QML_AUTH_SETTING_FIELD_H
