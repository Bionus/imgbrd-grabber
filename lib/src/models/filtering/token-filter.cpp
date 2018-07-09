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

QString TokenFilter::match(const QMap<QString, Token> &tokens, bool invert) const
{
	if (m_invert)
	{ invert = !invert; }

	const bool cond = tokens.contains(m_token) && !isVariantEmpty(tokens[m_token].value());

	if (cond && invert)
	{ return QObject::tr("image has a \"%1\" token").arg(m_token); }
	else if (!cond && !invert)
	{ return QObject::tr("image does not have a \"%1\" token").arg(m_token); }

	return QString();
}
