#include "image-downloader.h"
#include "logger.h"


ImageDownloader::ImageDownloader(QSharedPointer<Image> img, QString filename, QString path, int count, bool addMd5, bool startCommands, QObject *parent)
	: ImageDownloader(img, m_image->path(filename, path, count, true, false, true, true, true), count, addMd5, startCommands, parent)
{}

ImageDownloader::ImageDownloader(QSharedPointer<Image> img, QStringList paths, int count, bool addMd5, bool startCommands, QObject *parent)
	: QObject(parent), m_image(img), m_paths(paths), m_fileDownloader(this), m_count(count), m_addMd5(addMd5), m_startCommands(startCommands)
{}

void ImageDownloader::save()
{
	QMap<QString, Image::SaveResult> result = m_image->save(m_paths, m_addMd5, m_startCommands, m_count, false, false);
	bool needLoading = false;
	for (Image::SaveResult res : result)
		if (res == Image::SaveResult::NotLoaded)
			needLoading = true;

	// If we don't need any loading, we can return already
	if (!needLoading)
	{
		emit saved(m_image, result);
		return;
	}

	// Load the image directly on the disk
	log(QString("Loading and saving image in <a href=\"file:///%1\">%1</a>").arg(m_paths.first()));
	connect(m_image.data(), &Image::finishedImage, this, &ImageDownloader::imageLoaded, Qt::UniqueConnection);
	m_image->loadImage(false);

	// If we can't start writing for some reason, return an error
	if (!m_fileDownloader.start(m_image->imageReply(), m_paths))
	{
		log("Unable to open file", Logger::Error);
		emit saved(m_image, makeMap(m_paths, Image::SaveResult::Error));
		return;
	}
}

QMap<QString, Image::SaveResult> ImageDownloader::makeMap(QStringList keys, Image::SaveResult value)
{
	QMap<QString, Image::SaveResult> res;
	for (QString key : keys)
		res.insert(key, value);
	return res;
}

void ImageDownloader::imageLoaded(QNetworkReply::NetworkError error, QString msg)
{
	// Handle network errors
	if (error != QNetworkReply::NoError)
	{
		if (error == QNetworkReply::ContentNotFoundError)
		{ emit saved(m_image, makeMap(m_paths, Image::SaveResult::NotFound)); }
		else
		{
			log(QString("Network error for the image: <a href=\"%1\">%1</a>: %2 (%3)").arg(m_image->url().toHtmlEscaped()).arg(error).arg(msg), Logger::Error);
			emit saved(m_image, makeMap(m_paths, Image::SaveResult::NetworkError));
		}
		return;
	}

	// Success!
	m_image->postSaving(m_paths.first(), m_addMd5, m_startCommands, m_count, false);
	emit saved(m_image, makeMap(m_paths, Image::SaveResult::Saved));
}
