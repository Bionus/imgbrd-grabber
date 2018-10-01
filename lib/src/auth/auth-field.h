#ifndef AUTH_FIELD_H
#define AUTH_FIELD_H

#include <QString>


class AuthField
{
	public:
		enum FieldType
		{
			Username,
			Password,
			Hash
		};

		AuthField(QString key, FieldType type);
		virtual ~AuthField() = default;

		QString key() const;
		FieldType type() const;
		virtual QString value(const QString &val) const;

	private:
		QString m_key;
		FieldType m_type;
};

#endif // AUTH_FIELD_H
