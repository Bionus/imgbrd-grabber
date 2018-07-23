#include "token-filter.h"
#include <QStringBuilder>
#include "functions.h"


TokenFilter::TokenFilter(QString token, bool invert)
	: Filter(invert), m_token(std::move(token))
{}

QString TokenFilter::toString() const
{
	return QString(m_invert ? "-" : "") % "%" % m_token % "%";
}

bool TokenFilter::compare(const Filter &rhs) const
{
	const auto other = dynamic_cast<const TokenFilter*>(&rhs);
	if (other == nullptr)
		return false;

	return m_token == other->m_token;
}

QString TokenFilter::match(const QMap<QString, Token> &tokens, bool invert) const
{
	if (m_invert)
	{ invert = !invert; }

	const bool cond = tokens.contains(m_token) && !isVariantEmpty(tokens[m_token].value());

	if (cond && invert)
	{ return QObject::tr("image has a \"%1\" token").arg(m_token); }
	if (!cond && !invert)
	{ return QObject::tr("image does not have a \"%1\" token").arg(m_token); }

	return QString();
}
