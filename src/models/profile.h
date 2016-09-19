#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QList>
#include <QSettings>
#include "favorite.h"


class Profile
{
	public:
		Profile(QString path);
		~Profile();
		void sync();

		void addFavorite(Favorite fav);
		void removeFavorite(Favorite fav);

		// Getters
		QString getPath() const;
		QSettings *getSettings() const;
		QList<Favorite> &getFavorites();
		QStringList &getKeptForLater();
		QStringList &getIgnored();

	private:
		QString 		m_path;
		QSettings		*m_settings;
		QList<Favorite>	m_favorites;
		QStringList		m_keptForLater;
		QStringList		m_ignored;
};

#endif // PROFILE_H
