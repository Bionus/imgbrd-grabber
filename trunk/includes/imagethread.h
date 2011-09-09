#ifndef IMAGETHREAD_H
#define IMAGETHREAD_H

#include <QtGui>
#include <QtNetwork>

class ImageThread : public QThread
{
	Q_OBJECT

	public:
		ImageThread(QByteArray data, QObject* parent = 0);

	protected:
		void run();

	signals:
		void finished(QPixmap, int);

	private:
		QByteArray m_data;
};

#endif // IMAGETHREAD_H
