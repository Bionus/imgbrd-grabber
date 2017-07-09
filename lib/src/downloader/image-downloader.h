#ifndef IMAGE_DOWNLOADER_H
#define IMAGE_DOWNLOADER_H

#include <QObject>
#include "models/image.h"
#include "file-downloader.h"


class ImageDownloader : public QObject
{
	Q_OBJECT

	public:
		explicit ImageDownloader(QSharedPointer<Image> img, QObject *parent = Q_NULLPTR);
		void save(QString filename, QString path, bool addMd5, bool startCommands, int count);

	protected:
		QMap<QString, Image::SaveResult> makeMap(QStringList keys, Image::SaveResult value);

	signals:
		void saved(QSharedPointer<Image> img, QMap<QString, Image::SaveResult> result);

	private slots:
		void imageLoaded(QNetworkReply::NetworkError error, QString msg);

	private:
		QSharedPointer<Image> m_image;
		QStringList m_paths;
		FileDownloader m_fileDownloader;
};

#endif // IMAGE_DOWNLOADER_H
