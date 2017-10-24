#include "token.h"
#include <QDateTime>


bool isVariantEmpty(const QVariant &value)
{
	switch (value.type())
	{
		case QVariant::Type::List: return value.toList().isEmpty();
		case QVariant::Type::StringList: return value.toStringList().isEmpty();
		case QVariant::Type::String: return value.toString().isEmpty();
		case QVariant::Type::Int: return value.toInt() == 0;
	}
	return false;
}

Token::Token(const QString &key, const QVariant &value, const QVariant &def)
	: m_key(key)
{
	m_value = value.isNull() || !value.isValid() || isVariantEmpty(value) ? def : value;
}

QString Token::key() const
{
	return m_key;
}

QVariant Token::value() const
{
	return m_value;
}
