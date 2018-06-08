#include "loader/downloadable.h"
#include "functions.h"
#include "models/profile.h"


const QMap<QString, Token> &Downloadable::tokens(Profile *profile) const
{
	if (m_tokens.isEmpty())
	{
		auto tokens = generateTokens(profile);

		// Custom tokens (if the tokens contain tags)
		if (tokens.contains("tags"))
		{
			QMap<QString, QStringList> scustom = getCustoms(profile->getSettings());
			QMap<QString, QStringList> custom;
			for (const QString &tag : tokens["tags"].value().toStringList())
			{
				for (auto it = scustom.begin(); it != scustom.end(); ++it)
				{
					const QString &key = it.key();
					if (!custom.contains(key))
					{ custom.insert(key, QStringList()); }
					if (it.value().contains(tag, Qt::CaseInsensitive))
					{ custom[key].append(tag); }
				}
			}
			for (auto it = custom.begin(); it != custom.end(); ++it)
			{ tokens.insert(it.key(), Token(it.value())); }
		}

		m_tokens = tokens;
	}

	return m_tokens;
}

void Downloadable::refreshTokens()
{
	m_tokens.clear();
}
