#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <QObject>
#include <QString>


class Profile;

class ImagePreview : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString previewUrl READ previewUrl CONSTANT)
	Q_PROPERTY(QString fileUrl READ fileUrl CONSTANT)
	Q_PROPERTY(QString tags READ tags CONSTANT)

	public:
		ImagePreview(QString previewUrl, QString fileUrl, QString tags, QObject *parent = nullptr) : QObject(parent), m_previewUrl(previewUrl), m_fileUrl(fileUrl), m_tags(tags) {}
		QString previewUrl() const { return m_previewUrl; }
		QString fileUrl() const { return m_fileUrl; }
		QString tags() const { return m_tags; }

	private:
		QString m_previewUrl;
		QString m_fileUrl;
		QString m_tags;
};

class MainScreen : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString query READ query NOTIFY queryChanged)
	Q_PROPERTY(QList<QObject*> results READ results NOTIFY resultsChanged)
	Q_PROPERTY(QString log READ log NOTIFY logChanged)
	Q_PROPERTY(QStringList sites READ sites NOTIFY sitesChanged)

	public:
		explicit MainScreen(Profile *profile, QObject *parent = nullptr);
		const QString &query() const { return m_query; }
		const QList<QObject*> &results() const { return m_results; }
		const QString &log() const { return m_log; }
		const QStringList &sites() const { return m_sites; }

	public slots:
		void search(const QString &site, const QString &query, int page);
		void newLog(const QString &message);

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
