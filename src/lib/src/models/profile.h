#ifndef PROFILE_H
#define PROFILE_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include "history/history.h"
#include "models/favorite.h"
#include "models/filtering/blacklist.h"
#include "models/filtering/tag-filter-list.h"


class Commands;
class DownloadQueryManager;
class Exiftool;
class Md5Database;
class MonitorManager;
class QSettings;
class Site;
class Source;
class SourceRegistry;
class UrlDownloaderManager;

class Profile : public QObject
{
	Q_OBJECT

	public:
		Profile();
		Profile(QSettings *settings, QList<Favorite> favorites, QStringList keptForLater = QStringList(), QString path = QString());
		explicit Profile(QString path);
		~Profile() override;

		// Sync
		void reload();
		void sync();
		void syncFavorites() const;
		void syncKeptForLater() const;
		void syncIgnored() const;
		void syncBlacklist() const;

		// Temporary path
		QString tempPath() const;
		void purgeTemp(int maxAge) const;

		// Favorite management
		void addFavorite(const Favorite &fav);
		void removeFavorite(const Favorite &fav);
		void emitFavorite();

		// KFL management
		void addKeptForLater(const QString &tag);
		void removeKeptForLater(const QString &tag);

		// Ignore management
		void setIgnored(const QStringList &tags);
		void addIgnored(const QString &tag);
		void removeIgnored(const QString &tag);

		// Removed tags management
		void setRemovedTags(const QString &raw);

		// MD5 management
		QPair<QString, QString> md5Action(const QString &md5, const QString &target);
		QStringList md5Exists(const QString &md5);
		void addMd5(const QString &md5, const QString &path);
		void removeMd5(const QString &md5, const QString &path = {});

		// Auto-completion
		void addAutoComplete(const QString &tag);
		QStringList addAutoComplete(const QList<Tag> &tags);

		// Sites management
		void addSource(Source *source);
		void addSite(Site *site);
		void removeSite(Site *site);

		// Blacklist management
		void setBlacklistedTags(const Blacklist &blacklist);
		void addBlacklistedTag(const QString &tag);
		void removeBlacklistedTag(const QString &tag);

		// Source registries
		const QList<SourceRegistry*> &getSourceRegistries() const;
		void addSourceRegistry(SourceRegistry *sourceRegistry);
		void removeSourceRegistry(SourceRegistry *sourceRegistry);
		void syncSourceRegistries();

		// Getters
		QString getPath() const;
		QSettings *getSettings() const;
		QList<Favorite> &getFavorites();
		QStringList &getKeptForLater();
		QStringList &getIgnored();
		TagFilterList &getRemovedTags();
		Commands &getCommands();
		Exiftool &getExiftool();
		QStringList &getAutoComplete();
		Blacklist &getBlacklist();
		History *getHistory();
		const QMap<QString, Source*> &getSources() const;
		const QMap<QString, Site*> &getSites() const;
		const QStringList &getAdditionalTokens() const;
		QList<Site*> getFilteredSites(const QStringList &urls) const;
		MonitorManager *monitorManager() const;
		DownloadQueryManager *downloadQueryManager() const;
		UrlDownloaderManager *urlDownloaderManager() const;
		Md5Database *md5Database() const;

	signals:
		void favoritesChanged();
		void keptForLaterChanged();
		void ignoredChanged();
		void sitesChanged();
		void siteDeleted(Site *site);
		void blacklistChanged();
		void sourceRegistriesChanged();

	private:
		QString m_path;
		QSettings *m_settings;
		QList<Favorite> m_favorites;
		QStringList m_keptForLater;
		QStringList m_ignored;
		TagFilterList m_removedTags;
		Commands *m_commands;
		Exiftool *m_exiftool;
		QStringList m_autoComplete;
		QStringList m_customAutoComplete;
		Blacklist m_blacklist;
		History *m_history;
		Md5Database *m_md5s;
		QMap<QString, Source*> m_sources;
		QMap<QString, Site*> m_sites;
		QStringList m_additionalTokens;
		MonitorManager *m_monitorManager;
		DownloadQueryManager *m_downloadQueryManager;
		UrlDownloaderManager *m_urlDownloaderManager;
		QList<SourceRegistry*> m_sourceRegistries;
};

#endif // PROFILE_H
