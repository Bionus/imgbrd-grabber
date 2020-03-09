#include "token.h"
#include <utility>
#include "functions.h"


Token::Token(const QVariant &value, const QVariant &def)
	: m_value(def.isValid() && (value.isNull() || !value.isValid() || isVariantEmpty(value)) ? def : value)
{}

Token::Token(QVariant value, QString whatToDoDefault, QString emptyDefault, QString multipleDefault)
	: m_value(std::move(value)), m_whatToDoDefault(std::move(whatToDoDefault)), m_emptyDefault(std::move(emptyDefault)), m_multipleDefault(std::move(multipleDefault))
{}

Token::Token(std::function<QVariant()> func, bool cacheResult)
	: m_func(std::move(func)), m_cacheResult(cacheResult)
{}


QVariant Token::value() const
{
	if (m_func == nullptr || m_value.isValid()) {
		return m_value;
	}

	QVariant val = m_func();
	if (m_cacheResult) {
		m_value = val;
	}

	return val;
}

QString Token::toString() const
{ return value().toString(); }


const QString &Token::whatToDoDefault() const
{ return m_whatToDoDefault; }

const QString &Token::emptyDefault() const
{ return m_emptyDefault; }

const QString &Token::multipleDefault() const
{ return m_multipleDefault; }


bool operator==(const Token &lhs, const Token &rhs)
{ return lhs.value() == rhs.value(); }

bool operator!=(const Token &lhs, const Token &rhs)
{ return !(lhs == rhs); }
