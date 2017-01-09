#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QList>
#include <QSettings>
#include "models/favorite.h"



class Commands;

class Profile : public QObject
{
	Q_OBJECT

	public:
		Profile();
		Profile(QSettings *settings, QList<Favorite> favorites, QStringList keptForLater = QStringList(), QString path = QString());
		Profile(QString path);
		~Profile();
		void sync();

		// Favorite management
		void addFavorite(Favorite fav);
		void removeFavorite(Favorite fav);
		void emitFavorite();

		// KFL management
		void addKeptForLater(QString tag);
		void removeKeptForLater(QString tag);

		// Ignore management
		void addIgnored(QString tag);
		void removeIgnored(QString tag);

		// MD5 management
		QString md5Exists(QString);
		void addMd5(QString, QString);
		void setMd5(QString, QString);
		void removeMd5(QString);

		// Auto-completion
		void addAutoComplete(QString tag);

		// Getters
		QString getPath() const;
		QSettings *getSettings() const;
		QList<Favorite> &getFavorites();
		QStringList &getKeptForLater();
		QStringList &getIgnored();
		Commands &getCommands();
		QStringList &getAutoComplete();
		QStringList &getCustomAutoComplete();

	signals:
		void favoritesChanged();
		void keptForLaterChanged();
		void ignoredChanged();

	private:
		QString 		m_path;
		QSettings		*m_settings;
		QList<Favorite>	m_favorites;
		QStringList		m_keptForLater;
		QStringList		m_ignored;
		Commands		*m_commands;
		QStringList		m_autoComplete;
		QStringList		m_customAutoComplete;
		QMap<QString, QString>	m_md5s;
};

#endif // PROFILE_H
