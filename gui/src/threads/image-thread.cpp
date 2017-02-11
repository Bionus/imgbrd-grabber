#include "image-thread.h"


ImageThread::ImageThread(QObject* parent)
	: QObject(parent)
{ }

void ImageThread::start(const QByteArray &data)
{
	QPixmap img;
	if (img.loadFromData(data))
		emit finished(img, data.size());
}
