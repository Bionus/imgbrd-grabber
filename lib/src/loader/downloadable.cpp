#include "loader/downloadable.h"
#include "functions.h"
#include "models/filename.h"
#include "models/profile.h"


const QMap<QString, Token> &Downloadable::tokens(Profile *profile) const
{
	if (m_tokens.isEmpty())
	{
		auto tokens = generateTokens(profile);

		// Custom tokens (if the tokens contain tags)
		if (tokens.contains("tags"))
		{
			const QStringList &tags = tokens["tags"].value().toStringList();
			QMap<QString, QStringList> scustom = getCustoms(profile->getSettings());
			QMap<QString, QStringList> custom;
			for (const QString &tag : tags)
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

		// Use a lazy token for Grabber meta-tags as it can be expensive to calculate
		tokens.insert("grabber", Token([profile, tokens]()
		{
			const QString pth = profile->getSettings()->value("Save/path").toString();
			Filename filename(profile->getSettings()->value("Save/filename").toString());
			QStringList paths = filename.path(tokens, profile, pth);
			bool alreadyExists = false;
			for (const QString &path : paths)
			{
				if (QFile::exists(path))
				{
					alreadyExists = true;
					break;
				}
			}
			const bool inMd5List = !profile->md5Action(tokens["md5"].value().toString()).second.isEmpty();

			// Generate corresponding combination
			QStringList metas;
			if (alreadyExists)
			{ metas.append("alreadyExists"); }
			if (inMd5List)
			{ metas.append("inMd5List"); }
			if (alreadyExists || inMd5List)
			{ metas.append("downloaded"); }
			return metas;
		}));

		m_tokens = tokens;
	}

	return m_tokens;
}

void Downloadable::refreshTokens()
{
	m_tokens.clear();
}
