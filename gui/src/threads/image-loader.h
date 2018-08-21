#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <QObject>


class QByteArray;
class QPixmap;

class ImageLoader : public QObject
{
	Q_OBJECT

	public:
		explicit ImageLoader(QObject *parent = nullptr);

	public slots:
		void load(const QByteArray &data);

	signals:
		void finished(const QPixmap &, int);
		void failed();
};

#endif // IMAGE_LOADER_H
