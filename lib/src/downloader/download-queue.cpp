#include "downloader/download-queue.h"
#include "concurrent-multi-queue.h"
#include "downloader/image-downloader.h"


DownloadQueue::DownloadQueue(int maxConcurrency, QObject *parent)
	: QObject(parent)
{
	m_queue = new ConcurrentMultiQueue(this);
	m_queue->setGlobalConcurrency(maxConcurrency);

	connect(m_queue, &ConcurrentMultiQueue::dequeued, this, &DownloadQueue::dequeued);
}


void DownloadQueue::add(Queue queue, ImageDownloader *downloader)
{
	QVariant variant = QVariant::fromValue(downloader);
	m_queue->append(static_cast<int>(queue), variant);
}

void DownloadQueue::dequeued(const QVariant &item)
{
	ImageDownloader *downloader = item.value<ImageDownloader*>();
	connect(downloader, &ImageDownloader::saved, m_queue, &ConcurrentMultiQueue::next);
	connect(downloader, &ImageDownloader::saved, downloader, &ImageDownloader::deleteLater);
	downloader->save();
}
