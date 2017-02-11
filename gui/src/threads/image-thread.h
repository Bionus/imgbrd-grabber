#ifndef IMAGE_THREAD_H
#define IMAGE_THREAD_H

#include <QPixmap>
#include <QByteArray>


class ImageThread : public QObject
{
	Q_OBJECT

	public:
		ImageThread(QObject *parent = Q_NULLPTR);

	public slots:
		void start(const QByteArray &data);

	signals:
		void finished(const QPixmap &, int);
};

#endif // IMAGE_THREAD_H
