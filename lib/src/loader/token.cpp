#include "token.h"
#include <QDateTime>
#include "functions.h"


Token::Token(const QVariant &value, const QVariant &def)
{
	m_value = def.isValid() && (value.isNull() || !value.isValid() || isVariantEmpty(value)) ? def : value;
}

Token::Token(const QVariant &value, const QString &whatToDoDefault, const QString &emptyDefault, const QString &multipleDefault)
	: m_value(value), m_whatToDoDefault(whatToDoDefault), m_emptyDefault(emptyDefault), m_multipleDefault(multipleDefault)
{}


QVariant Token::value() const
{ return m_value; }

QString Token::toString() const
{ return m_value.toString(); }


QString Token::whatToDoDefault() const
{ return m_whatToDoDefault; }

QString Token::emptyDefault() const
{ return m_emptyDefault; }

QString Token::multipleDefault() const
{ return m_multipleDefault; }


bool operator==(const Token &lhs, const Token &rhs)
{ return lhs.value() == rhs.value(); }

bool operator!=(const Token &lhs, const Token &rhs)
{ return !(lhs == rhs); }
