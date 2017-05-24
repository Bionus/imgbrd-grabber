#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QList>
#include <QSettings>
#include <QPair>
#include "models/favorite.h"


class Commands;
class Site;

class Profile : public QObject
{
	Q_OBJECT

	public:
		Profile();
		Profile(QSettings *settings, QList<Favorite> favorites, QStringList keptForLater = QStringList(), QString path = QString());
		Profile(QString path);
		~Profile();
		void sync();

		// Temporary path
		QString tempPath() const;

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
		QPair<QString, QString> md5Action(QString md5);
		QString md5Exists(QString md5);
		void addMd5(QString md5, QString path);
		void setMd5(QString md5, QString path);
		void removeMd5(QString md5);

		// Auto-completion
		void addAutoComplete(QString tag);

		// Sites management
		void addSite(Site *site);

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
		void sitesChanged();

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
