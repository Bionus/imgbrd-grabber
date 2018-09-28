#ifndef IMAGE_DOWNLOADER_H
#define IMAGE_DOWNLOADER_H

#include <QMap>
#include <QNetworkReply>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QUrl>
#include "downloader/file-downloader.h"
#include "loader/downloadable.h"
#include "models/image.h"


class Profile;

class ImageDownloader : public QObject
{
	Q_OBJECT

	public:
		ImageDownloader(Profile *profile, QSharedPointer<Image> img, QString filename, QString path, int count, bool addMd5, bool startCommands, bool getBlacklisted, QObject *parent = nullptr, bool loadTags = true, bool rotate = true, bool force = false);
		ImageDownloader(Profile *profile, QSharedPointer<Image> img, QStringList paths, int count, bool addMd5, bool startCommands, bool getBlacklisted, QObject *parent = nullptr, bool rotate = true, bool force = false);
		~ImageDownloader();
		bool isRunning() const;

	public slots:
		void save();
		void abort();

	protected:
		QMap<QString, Image::SaveResult> makeMap(const QStringList &keys, Image::SaveResult value);
		QMap<QString, Downloadable::SaveResult> postSaving(Image::SaveResult saveResult = Image::SaveResult::Saved);

	signals:
		void downloadProgress(QSharedPointer<Image> img, qint64 v1, qint64 v2);
		void saved(QSharedPointer<Image> img, const QMap<QString, Image::SaveResult> &result);

	private slots:
		void loadedSave();
		void loadImage();
		void downloadProgressImage(qint64 v1, qint64 v2);
		void writeError();
		void networkError(QNetworkReply::NetworkError error, const QString &msg);
		void success();

	private:
		Profile *m_profile;
		QSharedPointer<Image> m_image;
		FileDownloader m_fileDownloader;
		QString m_filename;
		QString m_path;
		bool m_loadTags;
		QStringList m_paths;
		QString m_temporaryPath;
		int m_count;
		bool m_addMd5;
		bool m_startCommands;
		bool m_getBlacklisted;
		bool m_writeError;
		bool m_rotate;
		bool m_force;

		QNetworkReply *m_reply = nullptr;
		QUrl m_url;
		bool m_tryingSample = false;
};

#endif // IMAGE_DOWNLOADER_H
