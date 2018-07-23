#include "threads/image-loader-queue.h"
#include <QObject>
#include "threads/image-loader.h"


ImageLoaderQueue::ImageLoaderQueue(ImageLoader *imageLoader, QObject *parent)
	: QObject(parent), m_next(QByteArray()), m_waiting(false), m_cancelNext(false), m_hasNext(false)
{
	connect(this, &ImageLoaderQueue::loadImage, imageLoader, &ImageLoader::load);
	connect(imageLoader, &ImageLoader::finished, this, &ImageLoaderQueue::loadingSuccess);
	connect(imageLoader, &ImageLoader::failed, this, &ImageLoaderQueue::loadingFinished);
}

void ImageLoaderQueue::clear()
{
	// If we have a loading waiting when we clear, we must not emit for it
	if (m_waiting)
	{
		m_cancelNext = true;
	}

	m_next = QByteArray();
	m_hasNext = false;
}

void ImageLoaderQueue::load(const QByteArray &data)
{
	// If we are already waiting for a loading, we queue this data
	if (m_waiting)
	{
		m_next = QByteArray(data);
		m_hasNext = true;
		return;
	}

	m_waiting = true;
	emit loadImage(data);
}

void ImageLoaderQueue::loadingSuccess(const QPixmap &pixmap, int size)
{
	// We only emit the event if the loading was successful and not cancelled
	if (!m_cancelNext)
	{
		emit finished(pixmap, size);
	}

	loadingFinished();
}

void ImageLoaderQueue::loadingFinished()
{
	m_waiting = false;
	m_cancelNext = false;

	// If we have some data in the queue, we load it directly
	if (m_hasNext)
	{
		load(m_next);
		m_next = QByteArray();
		m_hasNext = false;
	}
}
