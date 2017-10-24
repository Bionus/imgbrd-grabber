#ifndef TOKEN_H
#define TOKEN_H

#include <QString>
#include <QVariant>


class Token
{
	public:
		explicit Token(const QString &key, const QVariant &value, const QVariant &def);
		QString key() const;
		QVariant value() const;

	private:
		QString m_key;
		QVariant m_value;
};

#endif // TOKEN_H
