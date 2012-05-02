#ifndef IMAGETHREAD_H
#define IMAGETHREAD_H

#include <QThread>
#include <QPixmap>

class ImageThread : public QThread
{
	Q_OBJECT

	public:
		ImageThread(QByteArray data, QObject* parent = 0);

	protected:
		void run();

    signals:
        void finished(QPixmap, int);
        void finished(QImage, int);

	private:
		QByteArray m_data;
};

#endif // IMAGETHREAD_H
