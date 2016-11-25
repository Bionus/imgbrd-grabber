#include "imagethread.h"


ImageThread::ImageThread(QByteArray data, QObject* parent)
	: QThread(parent), m_data(data)
{ }

void ImageThread::run()
{
	QPixmap *img = new QPixmap();
	if (img->loadFromData(m_data))
		emit finished(img, m_data.size());
}
