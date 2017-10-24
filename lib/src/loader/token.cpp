#include "token.h"
#include <QDateTime>
#include "functions.h"


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
