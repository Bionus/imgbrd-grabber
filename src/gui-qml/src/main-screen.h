#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <QObject>
#include <QSharedPointer>
#include <QString>


class Image;
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
	Q_PROPERTY(QString settingsFileName READ settingsFileName CONSTANT)

	public:
		explicit MainScreen(Profile *profile, QObject *parent = nullptr);
		const QString &query() const { return m_query; }
		const QList<QmlImage*> &results() const { return m_results; }
		const QString &log() const { return m_log; }
		const QList<QmlSite*> &sites() const { return m_sites; }
		const QStringList &sources() const { return m_sources; }
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

	protected slots:
		void refreshSites();
		void refreshSources();

	signals:
		void queryChanged();
		void resultsChanged();
		void logChanged();
		void sitesChanged();
		void sourcesChanged();

	private:
		Profile *m_profile;
		QString m_query;
		QList<QmlImage*> m_results;
		QString m_log;
		QList<QmlSite*> m_sites;
		QStringList m_sources;
};

#endif // MAIN_SCREEN_H
