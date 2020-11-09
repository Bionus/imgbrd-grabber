#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <QObject>
#include <QSharedPointer>
#include <QString>


class Image;
class Page;
class Profile;
class QmlImage;
class QmlSite;
class Settings;

class MainScreen : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString query READ query NOTIFY queryChanged)
	Q_PROPERTY(QList<QmlImage*> results READ results NOTIFY resultsChanged)
	Q_PROPERTY(QString log READ log NOTIFY logChanged)
	Q_PROPERTY(QList<QmlSite*> sites READ sites NOTIFY sitesChanged)
	Q_PROPERTY(QStringList sources READ sources NOTIFY sourcesChanged)
	Q_PROPERTY(QStringList favorites READ favorites NOTIFY favoritesChanged)
	Q_PROPERTY(QStringList autoComplete READ autoComplete NOTIFY autoCompleteChanged)
	Q_PROPERTY(QString settingsFileName READ settingsFileName CONSTANT)

	public:
		explicit MainScreen(Profile *profile, QObject *parent = nullptr);
		const QString &query() const { return m_query; }
		const QList<QmlImage*> &results() const { return m_results; }
		const QString &log() const { return m_log; }
		const QList<QmlSite*> &sites() const { return m_sites; }
		const QStringList &sources() const { return m_sources; }
		const QStringList &favorites() const { return m_favorites; }
		const QStringList &autoComplete() const { return m_autoComplete; }
		QString settingsFileName() const;

	public slots:
		void search(const QString &site, const QString &query, int page, const QString &postFilter);
		void newLog(const QString &message);
		void downloadImage(const QSharedPointer<Image> &image);
		QString addSite(const QString &type, const QString &host, bool https);
		QString toLocalFile(const QString &url);
		QString getBlacklist();
		void setBlacklist(const QString &blacklist);
		QString getIgnored();
		void setIgnored(const QString &ignored);
		void addFavorite(const QString &query, const QString &siteUrl);
		void removeFavorite(const QString &query);
		void loadSuggestions(const QString &prefix, int limit);

	protected slots:
		void refreshSites();
		void refreshSources();
		void refreshFavorites();
		void searchFinished(Page *page);

	signals:
		void queryChanged();
		void resultsChanged();
		void logChanged();
		void sitesChanged();
		void sourcesChanged();
		void favoritesChanged();
		void autoCompleteChanged();

	private:
		Profile *m_profile;
		QString m_query;
		QList<QmlImage*> m_results;
		QString m_log;
		QList<QmlSite*> m_sites;
		QStringList m_sources;
		QStringList m_favorites;
		QStringList m_fullAutoComplete;
		QStringList m_autoComplete;
};

#endif // MAIN_SCREEN_H
