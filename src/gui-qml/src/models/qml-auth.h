#ifndef QML_AUTH_H
#define QML_AUTH_H

#include <QList>
#include <QObject>
#include <QString>
#include "auth/auth.h"
#include "models/qml-auth-setting-field.h"


class QmlAuth : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString id READ id CONSTANT)
	Q_PROPERTY(QString type READ type CONSTANT)
	Q_PROPERTY(QList<QmlAuthSettingField*> fields READ fields CONSTANT)

	public:
		explicit QmlAuth(QString id, Auth *auth, QObject *parent = nullptr)
			: QObject(parent), m_id(std::move(id)), m_type(auth->type())
		{
			for (const auto &field : auth->settingFields()) {
				m_fields.append(new QmlAuthSettingField(field, this));
			}
		}

		QString id() const { return m_id; }
		QString type() const { return m_type; }
		QList<QmlAuthSettingField*> fields() const { return m_fields; }

	private:
		QString m_id;
		QString m_type;
		QList<QmlAuthSettingField*> m_fields;
};

#endif // QML_AUTH_H
