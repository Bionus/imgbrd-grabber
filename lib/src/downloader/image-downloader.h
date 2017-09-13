#ifndef IMAGE_DOWNLOADER_H
#define IMAGE_DOWNLOADER_H

#include <QObject>
#include "models/image.h"
#include "file-downloader.h"


class ImageDownloader : public QObject
{
	Q_OBJECT

	public:
		ImageDownloader(QSharedPointer<Image> img, QString filename, QString path, int count, bool addMd5, bool startCommands, QObject *parent = Q_NULLPTR, bool loadTags = false);
		ImageDownloader(QSharedPointer<Image> img, QStringList paths, int count, bool addMd5, bool startCommands, QObject *parent = Q_NULLPTR);
		void save();

	protected:
		QMap<QString, Image::SaveResult> makeMap(QStringList keys, Image::SaveResult value);

	signals:
		void saved(QSharedPointer<Image> img, QMap<QString, Image::SaveResult> result);

	private slots:
		void loadedSave();
		void writeError();
		void imageLoaded(QNetworkReply::NetworkError error, QString msg);

	private:
		QSharedPointer<Image> m_image;
		FileDownloader m_fileDownloader;
		QString m_filename;
		QString m_path;
		bool m_loadTags;
		QStringList m_paths;
		int m_count;
		bool m_addMd5;
		bool m_startCommands;
		bool m_writeError;
};

#endif // IMAGE_DOWNLOADER_H
