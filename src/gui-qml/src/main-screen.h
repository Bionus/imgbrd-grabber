#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <QObject>
#include <QSharedPointer>
#include <QString>


class Image;
class Profile;

class ImagePreview : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString previewUrl READ previewUrl CONSTANT)
	Q_PROPERTY(QString sampleUrl READ sampleUrl CONSTANT)
	Q_PROPERTY(QString fileUrl READ fileUrl CONSTANT)
	Q_PROPERTY(QStringList tags READ tags CONSTANT)
	Q_PROPERTY(QSharedPointer<Image> image READ image CONSTANT)

	public:
		ImagePreview(QString previewUrl, QString sampleUrl, QString fileUrl, QStringList tags, QSharedPointer<Image> image, QObject *parent = nullptr)
			: QObject(parent), m_previewUrl(previewUrl), m_sampleUrl(sampleUrl), m_fileUrl(fileUrl), m_tags(tags), m_image(image) {}
		QString previewUrl() const { return m_previewUrl; }
		QString sampleUrl() const { return m_sampleUrl; }
		QString fileUrl() const { return m_fileUrl; }
		QStringList tags() const { return m_tags; }
		QSharedPointer<Image> image() const { return m_image; }

	private:
		QString m_previewUrl;
		QString m_sampleUrl;
		QString m_fileUrl;
		QStringList m_tags;
		QSharedPointer<Image> m_image;
};

class MainScreen : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString query READ query NOTIFY queryChanged)
	Q_PROPERTY(QList<QObject*> results READ results NOTIFY resultsChanged)
	Q_PROPERTY(QString log READ log NOTIFY logChanged)
	Q_PROPERTY(QStringList sites READ sites NOTIFY sitesChanged)
	Q_PROPERTY(QString settingsFileName READ settingsFileName CONSTANT)

	public:
		explicit MainScreen(Profile *profile, QObject *parent = nullptr);
		const QString &query() const { return m_query; }
		const QList<QObject*> &results() const { return m_results; }
		const QString &log() const { return m_log; }
		const QStringList &sites() const { return m_sites; }
		QString settingsFileName() const;

	public slots:
		void search(const QString &site, const QString &query, int page);
		void newLog(const QString &message);
		void downloadImage(const QSharedPointer<Image> &image);

	signals:
		void queryChanged();
		void resultsChanged();
		void logChanged();
		void sitesChanged();

	private:
		Profile *m_profile;
		QString m_query;
		QList<QObject*> m_results;
		QString m_log;
		QStringList m_sites;
};

#endif // MAIN_SCREEN_H
