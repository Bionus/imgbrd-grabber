#include "imagethread.h"

ImageThread::ImageThread(QByteArray data, QObject* parent) : QThread(parent), m_data(data)
{
}

void ImageThread::run()
{
	QImage image;
	if (image.loadFromData(m_data))
	{ emit finished(image, m_data.size()); }
}
