#include "loader/downloadable.h"


const QMap<QString, Token> &Downloadable::tokens(Profile *profile) const
{
	if (m_tokens.isEmpty())
		m_tokens = generateTokens(profile);

	return m_tokens;
}

void Downloadable::refreshTokens()
{
	m_tokens.clear();
}
