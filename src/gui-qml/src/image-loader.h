#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <downloader/image-save-result.h>
#include "models/image.h"


class ImageLoader : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QSharedPointer<Image> image READ image WRITE setImage NOTIFY imageChanged)
	Q_PROPERTY(ImageLoader::Size size READ size WRITE setSize NOTIFY sizeChanged)

	Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
	Q_PROPERTY(ImageLoader::Status status READ status NOTIFY statusChanged)
	Q_PROPERTY(QString error READ error NOTIFY errorChanged)
	Q_PROPERTY(QString source READ source NOTIFY sourceChanged)

	public:
		enum Status
		{
			Null,
			Ready,
			Loading,
			Error
		};
		Q_ENUM(Status)

		enum Size
		{
			Thumbnail,
			Sample,
			Full
		};
		Q_ENUM(Size)

		explicit ImageLoader(QObject *parent = nullptr);

		QSharedPointer<Image> image() const;
		void setImage(QSharedPointer<Image> image);

		ImageLoader::Size size() const;
		void setSize(ImageLoader::Size size);

		qreal progress() const;
		ImageLoader::Status status() const;
		QString error() const;
		QString source() const;

		Q_INVOKABLE void load();

	protected:
		Image::Size imageSize() const;
		void setError(QString error);
		void setSource(QString source);

	protected slots:
		void downloadProgress(const QSharedPointer<Image> &img, qint64 v1, qint64 v2);
		void saved(const QSharedPointer<Image> &img, const QList<ImageSaveResult> &result);

	signals:
		void imageChanged();
		void sizeChanged();
		void progressChanged();
		void statusChanged();
		void errorChanged();
		void sourceChanged();

	private:
		QSharedPointer<Image> m_image;
		ImageLoader::Size m_size;

		qreal m_progress;
		ImageLoader::Status m_status;
		QString m_error;
		QString m_source;
};

#endif // IMAGE_LOADER_H
