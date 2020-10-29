#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <QObject>
#include <QSharedPointer>
#include <QString>


class Image;
class Profile;
class Settings;

class ImagePreview : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString previewUrl READ previewUrl CONSTANT)
	Q_PROPERTY(QString sampleUrl READ sampleUrl CONSTANT)
	Q_PROPERTY(QString fileUrl READ fileUrl CONSTANT)
	Q_PROPERTY(QStringList tags READ tags CONSTANT)
	Q_PROPERTY(QSharedPointer<Image> image READ image CONSTANT)
	Q_PROPERTY(bool isAnimated READ isAnimated CONSTANT)
	Q_PROPERTY(bool isVideo READ isVideo CONSTANT)

	public:
		ImagePreview(QString previewUrl, QString sampleUrl, QString fileUrl, QStringList tags, QSharedPointer<Image> image, bool isAnimated, bool isVideo, QObject *parent = nullptr)
			: QObject(parent), m_previewUrl(previewUrl), m_sampleUrl(sampleUrl), m_fileUrl(fileUrl), m_tags(tags), m_image(image), m_isAnimated(isAnimated), m_isVideo(isVideo) {}
		QString previewUrl() const { return m_previewUrl; }
		QString sampleUrl() const { return m_sampleUrl; }
		QString fileUrl() const { return m_fileUrl; }
		QStringList tags() const { return m_tags; }
		QSharedPointer<Image> image() const { return m_image; }
		bool isAnimated() const { return m_isAnimated; }
		bool isVideo() const { return m_isVideo; }

	private:
		QString m_previewUrl;
		QString m_sampleUrl;
		QString m_fileUrl;
		QStringList m_tags;
		QSharedPointer<Image> m_image;
		bool m_isAnimated;
		bool m_isVideo;
};

class MainScreen : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString query READ query NOTIFY queryChanged)
	Q_PROPERTY(QList<QObject*> results READ results NOTIFY resultsChanged)
	Q_PROPERTY(QString log READ log NOTIFY logChanged)
	Q_PROPERTY(QStringList sites READ sites NOTIFY sitesChanged)
	Q_PROPERTY(QStringList sources READ sources NOTIFY sourcesChanged)
	Q_PROPERTY(QString settingsFileName READ settingsFileName CONSTANT)

	public:
		explicit MainScreen(Profile *profile, QObject *parent = nullptr);
		const QString &query() const { return m_query; }
		const QList<QObject*> &results() const { return m_results; }
		const QString &log() const { return m_log; }
		const QStringList &sites() const { return m_sites; }
		const QStringList &sources() const { return m_sources; }
		QString settingsFileName() const;

	public slots:
		void search(const QString &site, const QString &query, int page);
		void newLog(const QString &message);
		void downloadImage(const QSharedPointer<Image> &image);
		QString addSite(const QString &type, const QString &host, bool https);
		Settings *getSiteSettings(const QString &url);
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
		QList<QObject*> m_results;
		QString m_log;
		QStringList m_sites;
		QStringList m_sources;
};

#endif // MAIN_SCREEN_H
