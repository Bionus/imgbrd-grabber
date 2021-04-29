#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include "loader.h"
#include <QSharedPointer>
#include <QString>
#include <downloader/image-save-result.h>
#include "models/image.h"


class ImageLoader : public Loader
{
	Q_OBJECT

	Q_PROPERTY(QSharedPointer<Image> image READ image WRITE setImage NOTIFY imageChanged)
	Q_PROPERTY(ImageLoader::Size size READ size WRITE setSize NOTIFY sizeChanged)
	Q_PROPERTY(bool automatic READ automatic WRITE setAutomatic NOTIFY automaticChanged)

	Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
	Q_PROPERTY(QString source READ source NOTIFY sourceChanged)

	public:
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

		bool automatic() const;
		void setAutomatic(bool automatic);

		qreal progress() const;
		QString source() const;

	public slots:
		void load() override;

	protected:
		Image::Size imageSize() const;
		void setSource(QString source);

	protected slots:
		void downloadProgress(const QSharedPointer<Image> &img, qint64 v1, qint64 v2);
		void saved(const QSharedPointer<Image> &img, const QList<ImageSaveResult> &result);

	signals:
		void imageChanged();
		void sizeChanged();
		void automaticChanged();
		void progressChanged();
		void sourceChanged();

	private:
		QSharedPointer<Image> m_image;
		ImageLoader::Size m_size;
		bool m_automatic;

		qreal m_progress;
		QString m_source;
};

#endif // IMAGE_LOADER_H
