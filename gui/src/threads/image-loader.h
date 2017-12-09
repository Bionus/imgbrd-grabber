#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <QByteArray>
#include <QPixmap>


class ImageLoader : public QObject
{
	Q_OBJECT

	public:
		explicit ImageLoader(QObject *parent = Q_NULLPTR);

	public slots:
		void load(const QByteArray &data);

	signals:
		void finished(const QPixmap &, int);
		void failed();
};

#endif // IMAGE_LOADER_H
