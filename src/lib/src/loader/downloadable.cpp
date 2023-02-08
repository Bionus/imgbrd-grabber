#include "loader/downloadable.h"
#include <QFile>
#include <QSet>
#include <QSettings>
#include <QStringList>
#include "filename/filename.h"
#include "functions.h"
#include "loader/token.h"
#include "models/profile.h"
#include "tags/tag.h"


bool isFavorited(const QList<Tag> &tags, const QList<Favorite> &favorites)
{
	for (const Tag &tag : tags) {
		for (const Favorite &fav : favorites) {
			if (fav.getName() == tag.text()) {
				return true;
			}
		}
	}
	return false;
}

const QMap<QString, Token> &Downloadable::tokens(Profile *profile) const
{
	if (m_tokens.isEmpty()) {
		auto tokens = generateTokens(profile);

		// Custom tokens (if the tokens contain tags)
		if (tokens.contains("tags")) {
			const QList<Tag> &tags = tokens["tags"].value<QList<Tag>>();
			QMap<QString, QStringList> scustom = getCustoms(profile->getSettings());
			QMap<QString, QStringList> custom;
			for (const Tag &tag : tags) {
				for (auto it = scustom.constBegin(); it != scustom.constEnd(); ++it) {
					const QString &key = it.key();
					if (!custom.contains(key)) {
						custom.insert(key, QStringList());
					}
					if (it.value().contains(tag.text(), Qt::CaseInsensitive)) {
						custom[key].append(tag.text());
					}
				}
			}
			for (auto it = custom.constBegin(); it != custom.constEnd(); ++it) {
				tokens.insert(it.key(), Token(it.value()));
			}
		}

		// Use a lazy token for Grabber meta-tags as it can be expensive to calculate
		tokens.insert("grabber", Token([profile, tokens]()
		{
			const QString pth = profile->getSettings()->value("Save/path").toString();
			Filename filename(profile->getSettings()->value("Save/filename").toString());
			QStringList paths = filename.path(tokens, profile, pth, 0, Filename::Full);
			bool alreadyExists = false;
			for (const QString &path : paths) {
				if (QFile::exists(path)) {
					alreadyExists = true;
					break;
				}
			}
			const bool inMd5List = !profile->md5Exists(tokens["md5"].value().toString()).isEmpty();

			// Generate corresponding combination
			QStringList metas;
			if (alreadyExists) {
				metas.append("alreadyExists");
			}
			if (inMd5List) {
				metas.append("inMd5List");
			}
			if (alreadyExists || inMd5List) {
				metas.append("downloaded");
			}

			if (tokens.contains("tags")) {
				// Convert tags to a set of strings to avoid O(n*m) complxity below
				QSet<QString> tags;
				for (const Tag &tag : tokens["tags"].value<QList<Tag>>()) {
					tags.insert(tag.text());
				}

				// Favorited
				const auto &favorites = profile->getFavorites();
				bool isFavorited = std::any_of(favorites.constBegin(), favorites.constEnd(), [&tags](const Favorite &fav) {
					return tags.contains(fav.getName());
				});
				if (isFavorited) {
					metas.append("favorited");
				}
			}

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
