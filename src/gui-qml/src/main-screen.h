#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <QObject>
#include <QSharedPointer>
#include <QString>

Q_MOC_INCLUDE("models/profile.h")
Q_MOC_INCLUDE("models/qml-site.h")


class Image;
class Page;
class Profile;
class QmlImage;
class QmlSite;
class Settings;
class ShareUtils;

class MainScreen : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString log READ log NOTIFY logChanged)
	Q_PROPERTY(QList<QmlSite*> sites READ sites NOTIFY sitesChanged)
	Q_PROPERTY(QStringList sources READ sources NOTIFY sourcesChanged)
	Q_PROPERTY(QStringList favorites READ favorites NOTIFY favoritesChanged)
	Q_PROPERTY(QStringList autoComplete READ autoComplete NOTIFY autoCompleteChanged)
	Q_PROPERTY(QString settingsFileName READ settingsFileName CONSTANT)
	Q_PROPERTY(Profile * profile READ profile CONSTANT)

	public:
		explicit MainScreen(Profile *profile, ShareUtils *shareUtils, QObject *parent = nullptr);
		const QString &log() const { return m_log; }
		const QList<QmlSite*> &sites() const { return m_sites; }
		const QStringList &sources() const { return m_sources; }
		const QStringList &favorites() const { return m_favorites; }
		const QStringList &autoComplete() const { return m_autoComplete; }
		QString settingsFileName() const;
		Profile *profile() const { return m_profile; }

	public slots:
		void newLog(const QString &message);
		void downloadImage(const QSharedPointer<Image> &image);
		void shareImage(const QSharedPointer<Image> &image);
		QString addSite(const QString &type, const QString &host, bool https);
		QString toLocalFile(const QString &url);
		QString getBlacklist();
		void setBlacklist(const QString &blacklist);
		QString getIgnored();
		void setIgnored(const QString &ignored);
		void addFavorite(const QString &query, const QString &siteUrl);
		void removeFavorite(const QString &query);
		void loadSuggestions(const QString &prefix, int limit);
		bool exportSettings(const QString &dest);
		bool importSettings(const QString &source);
		bool removeSite(QmlSite *site);
		void setClipboardText(const QString &text);

	protected slots:
		void refreshSites();
		void refreshSources();
		void refreshFavorites();

	signals:
		void logChanged();
		void sitesChanged();
		void sourcesChanged();
		void favoritesChanged();
		void autoCompleteChanged();
		void settingsChanged();

	private:
		Profile *m_profile;
		ShareUtils *m_shareUtils;
		QString m_log;
		QList<QmlSite*> m_sites;
		QStringList m_sources;
		QStringList m_favorites;
		QStringList m_fullAutoComplete;
		QStringList m_autoComplete;
};

#endif // MAIN_SCREEN_H
