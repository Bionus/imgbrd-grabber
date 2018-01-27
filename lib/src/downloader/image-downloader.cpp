#include "downloader/image-downloader.h"
#include <QUuid>
#include "logger.h"
#include "models/filename.h"


ImageDownloader::ImageDownloader(QSharedPointer<Image> img, const QString &filename, const QString &path, int count, bool addMd5, bool startCommands, QObject *parent, bool loadTags)
	: QObject(parent), m_fileDownloader(this), m_image(img), m_filename(filename), m_path(path), m_loadTags(loadTags), m_count(count), m_addMd5(addMd5), m_startCommands(startCommands), m_writeError(false)
{}

ImageDownloader::ImageDownloader(QSharedPointer<Image> img, const QStringList &paths, int count, bool addMd5, bool startCommands, QObject *parent)
	: QObject(parent), m_fileDownloader(this), m_image(img), m_paths(paths), m_count(count), m_addMd5(addMd5), m_startCommands(startCommands), m_writeError(false)
{}

void ImageDownloader::save()
{
	// If we use direct saving or don't want to load tags, we directly save the image
	if (!m_loadTags || !m_paths.isEmpty() || !Filename(m_filename).needExactTags(m_image->parentSite()))
	{
		loadedSave();
		return;
	}

	connect(m_image.data(), &Image::finishedLoadingTags, this, &ImageDownloader::loadedSave);
	m_image->loadDetails();
}

void ImageDownloader::loadedSave()
{
	m_temporaryPath = m_path + "/" + QUuid::createUuid().toString().mid(1, 36) + ".tmp";

	if (m_paths.isEmpty())
		m_paths = m_image->path(m_filename, m_path, m_count, true, false, true, true, true);

	QMap<QString, Image::SaveResult> result = m_image->save(QStringList() << m_temporaryPath, m_addMd5, m_startCommands, m_count, false, false);
	bool needLoading = false;
	bool saveOk = false;
	for (Image::SaveResult res : result)
	{
		if (res == Image::SaveResult::NotLoaded)
			needLoading = true;
		else if (res == Image::SaveResult::Saved || res == Image::SaveResult::Copied || res == Image::SaveResult::Moved)
			saveOk = true;
	}

	// If we don't need any loading, we can return already
	if (!needLoading)
	{
		if (saveOk)
		{ postSaving(); }
		emit saved(m_image, result);
		return;
	}

	// Load the image directly on the disk
	log(QString("Loading and saving image in <a href=\"file:///%1\">%1</a>").arg(m_paths.first()));
	connect(&m_fileDownloader, &FileDownloader::success, this, &ImageDownloader::success, Qt::UniqueConnection);
	connect(&m_fileDownloader, &FileDownloader::networkError, this, &ImageDownloader::networkError, Qt::UniqueConnection);
	connect(&m_fileDownloader, &FileDownloader::writeError, this, &ImageDownloader::writeError, Qt::UniqueConnection);
	m_image->loadImage(false);

	// If we can't start writing for some reason, return an error
	if (!m_fileDownloader.start(m_image->imageReply(), QStringList() << m_temporaryPath))
	{
		log("Unable to open file", Logger::Error);
		emit saved(m_image, makeMap(m_paths, Image::SaveResult::Error));
		return;
	}
}

QMap<QString, Image::SaveResult> ImageDownloader::makeMap(const QStringList &keys, Image::SaveResult value)
{
	QMap<QString, Image::SaveResult> res;
	for (const QString &key : keys)
		res.insert(key, value);
	return res;
}

void ImageDownloader::writeError()
{
	m_image->abortImage();
	emit saved(m_image, makeMap(m_paths, Image::SaveResult::Error));
}

void ImageDownloader::networkError(QNetworkReply::NetworkError error, const QString &msg)
{
	if (error == QNetworkReply::ContentNotFoundError)
	{ emit saved(m_image, makeMap(m_paths, Image::SaveResult::NotFound)); }
	else if (error != QNetworkReply::OperationCanceledError)
	{
		log(QString("Network error for the image: <a href=\"%1\">%1</a>: %2 (%3)").arg(m_image->url().toHtmlEscaped()).arg(error).arg(msg), Logger::Error);
		emit saved(m_image, makeMap(m_paths, Image::SaveResult::NetworkError));
	}
}

void ImageDownloader::success()
{
	postSaving();
	emit saved(m_image, makeMap(m_paths, Image::SaveResult::Saved));
}

void ImageDownloader::postSaving()
{
	m_image->setSavePath(m_temporaryPath);

	if (!m_filename.isEmpty())
	{ m_paths = m_image->path(m_filename, m_path, m_count, true, false, true, true, true); }

	QFile tmp(m_temporaryPath);
	tmp.rename(m_paths.first());
	for (int i = 1; i < m_paths.count(); ++i)
	{ tmp.copy(m_paths[i]); }

	for (const QString &path : m_paths)
	{ m_image->postSaving(path, m_addMd5, m_startCommands, m_count, false); }
}
