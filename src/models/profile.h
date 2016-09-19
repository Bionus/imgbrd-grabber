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

		// Getters
		QString getPath() const;
		QSettings *getSettings() const;
		const QList<Favorite> &getFavorites() const;
		const QList<QString> &getKeptForLater() const;

	private:
		QString 		m_path;
		QSettings		*m_settings;
		QList<Favorite>	m_favorites;
		QList<QString>	m_keptForLater;
};

#endif // PROFILE_H
