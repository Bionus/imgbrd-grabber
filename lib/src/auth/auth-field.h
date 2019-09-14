#ifndef AUTH_FIELD_H
#define AUTH_FIELD_H

#include <QString>


class MixedSettings;

class AuthField
{
	public:
		enum FieldType
		{
			Text,
			Password,
			Hash,
			Const
		};

		AuthField(QString id, QString key, FieldType type, QString def = QString());
		virtual ~AuthField() = default;

		QString id() const;
		QString key() const;
		FieldType type() const;

		virtual QString value(MixedSettings *settings) const;

	private:
		QString m_id;
		QString m_key;
		FieldType m_type;
		QString m_default;
};

#endif // AUTH_FIELD_H
