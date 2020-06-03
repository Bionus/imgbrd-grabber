#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <QObject>
#include <QString>


class Profile;

class ImagePreview : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString previewUrl READ previewUrl CONSTANT)

	public:
		ImagePreview(QString previewUrl, QObject *parent = nullptr) : QObject(parent), m_previewUrl(previewUrl) {}
		QString previewUrl() const { return m_previewUrl; }

	private:
		QString m_previewUrl;
};

class MainScreen : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString query READ query NOTIFY queryChanged)
	Q_PROPERTY(QList<QObject*> thumbnails READ thumbnails NOTIFY thumbnailsChanged)

	public:
		explicit MainScreen(Profile *profile, QObject *parent = nullptr);
		const QString &query() const { return m_query; }
		const QList<QObject*> &thumbnails() const { return m_thumbnails; }

	public slots:
		void search(const QString &query, int page);

	signals:
		void queryChanged();
		void thumbnailsChanged();

	private:
		Profile *m_profile;
		QString m_query;
		QList<QObject*> m_thumbnails;
};

#endif // MAIN_SCREEN_H
