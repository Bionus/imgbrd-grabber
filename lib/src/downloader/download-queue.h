#ifndef DOWNLOAD_QUEUE_H
#define DOWNLOAD_QUEUE_H

#include <QObject>
#include <QSharedPointer>


class ConcurrentMultiQueue;
class ImageDownloader;

class DownloadQueue : public QObject
{
	Q_OBJECT

	public:
		enum Queue
		{
			Manual = 0,
			Batch = 1,
			Background = 2,
		};

		explicit DownloadQueue(int maxConcurrency, QObject *parent = nullptr);
		void add(Queue queue, ImageDownloader *downloader);

	signals:
		void finished();

	protected slots:
		void dequeued(const QVariant &item);

	private:
		ConcurrentMultiQueue *m_queue;
};

#endif // DOWNLOAD_QUEUE_H
