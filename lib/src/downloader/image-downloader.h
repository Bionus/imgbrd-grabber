#ifndef IMAGE_DOWNLOADER_H
#define IMAGE_DOWNLOADER_H

#include <QObject>
#include "downloader/file-downloader.h"
#include "models/image.h"


class ImageDownloader : public QObject
{
	Q_OBJECT

	public:
		ImageDownloader(QSharedPointer<Image> img, const QString &filename, const QString &path, int count, bool addMd5, bool startCommands, QObject *parent = Q_NULLPTR, bool loadTags = false);
		ImageDownloader(QSharedPointer<Image> img, const QStringList &paths, int count, bool addMd5, bool startCommands, QObject *parent = Q_NULLPTR);

	public slots:
		void save();

	protected:
		QMap<QString, Image::SaveResult> makeMap(const QStringList &keys, Image::SaveResult value);
		void postSaving();

	signals:
		void saved(QSharedPointer<Image> img, const QMap<QString, Image::SaveResult> &result);

	private slots:
		void loadedSave();
		void loadImage();
		void writeError();
		void networkError(QNetworkReply::NetworkError error, const QString &msg);
		void success();

	private:
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
		bool m_writeError;

		QNetworkReply *m_reply = Q_NULLPTR;
		QString m_url = "";
		bool m_tryingSample = false;
};

#endif // IMAGE_DOWNLOADER_H
