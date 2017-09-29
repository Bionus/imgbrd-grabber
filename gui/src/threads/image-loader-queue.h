#ifndef IMAGE_LOADER_QUEUE_H
#define IMAGE_LOADER_QUEUE_H

#include <QPixmap>
#include <QByteArray>


class ImageLoader;

class ImageLoaderQueue : public QObject
{
	Q_OBJECT

	public:
		explicit ImageLoaderQueue(ImageLoader *imageLoader, QObject *parent = Q_NULLPTR);

	public slots:
		void load(const QByteArray &data);
		void clear();

	private slots:
		void loadingSuccess(const QPixmap &, int);
		void loadingFinished();

	signals:
		void finished(const QPixmap &, int);
		void loadImage(const QByteArray &);

	private:
		QByteArray m_next;
		bool m_waiting;
		bool m_cancelNext;
		bool m_hasNext;
};

#endif // IMAGE_LOADER_QUEUE_H
