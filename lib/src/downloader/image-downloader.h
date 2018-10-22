#ifndef IMAGE_DOWNLOADER_H
#define IMAGE_DOWNLOADER_H

#include <QList>
#include <QNetworkReply>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QUrl>
#include "downloader/file-downloader.h"
#include "downloader/image-save-result.h"
#include "loader/downloadable.h"
#include "models/filename.h"
#include "models/image.h"


class Profile;

class ImageDownloader : public QObject
{
	Q_OBJECT

	public:
		ImageDownloader(Profile *profile, QSharedPointer<Image> img, QString filename, QString path, int count, bool addMd5, bool startCommands, bool getBlacklisted, QObject *parent = nullptr, bool loadTags = true, bool rotate = true, bool force = false, Image::Size size = Image::Size::Unknown);
		ImageDownloader(Profile *profile, QSharedPointer<Image> img, QStringList paths, int count, bool addMd5, bool startCommands, bool getBlacklisted, QObject *parent = nullptr, bool rotate = true, bool force = false, Image::Size size = Image::Size::Unknown);
		~ImageDownloader();
		bool isRunning() const;
		void setSize(Image::Size size);

	public slots:
		void save();
		void abort();

	protected:
		int needExactTags(QSettings *settings) const;
		Image::Size currentSize() const;
		QList<ImageSaveResult> makeResult(const QStringList &paths, Image::SaveResult result) const;
		QList<ImageSaveResult> postSaving(Image::SaveResult saveResult = Image::SaveResult::Saved);

	signals:
		void downloadProgress(QSharedPointer<Image> img, qint64 v1, qint64 v2);
		void saved(QSharedPointer<Image> img, const QList<ImageSaveResult> &result);

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
		Filename m_filename;
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
		Image::Size m_size;

		QNetworkReply *m_reply = nullptr;
		QUrl m_url;
		bool m_tryingSample = false;
};

#endif // IMAGE_DOWNLOADER_H
