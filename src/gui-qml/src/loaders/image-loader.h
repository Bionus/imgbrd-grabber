#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include "loader.h"
#include <QQmlParserStatus>
#include <QSharedPointer>
#include <QString>
#include <downloader/image-save-result.h>
#include "models/image.h"


class ImageLoader : public Loader, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)

	Q_PROPERTY(QSharedPointer<Image> image READ image WRITE setImage NOTIFY imageChanged)
	Q_PROPERTY(ImageLoader::Size size READ size WRITE setSize NOTIFY sizeChanged)
	Q_PROPERTY(bool automatic READ automatic WRITE setAutomatic NOTIFY automaticChanged)
	Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
	Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

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
		void classBegin() override {}
		void componentComplete() override;

		QSharedPointer<Image> image() const;
		void setImage(QSharedPointer<Image> image);

		ImageLoader::Size size() const;
		void setSize(ImageLoader::Size size);

		bool automatic() const;
		void setAutomatic(bool automatic);

		QString filename() const { return m_filename; }
		void setFilename(QString filename) { m_filename = std::move(filename); emit filenameChanged(); }

		QString path() const { return m_path; }
		void setPath(QString path) { m_path = std::move(path); emit pathChanged(); }

		qreal progress() const;
		QString source() const;

	public slots:
		void load() override;
		void remove();

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
		void filenameChanged();
		void pathChanged();

	private:
		QSharedPointer<Image> m_image;
		ImageLoader::Size m_size;
		bool m_automatic;
		QString m_filename;
		QString m_path;

		bool m_loaded = false;
		qreal m_progress;
		QString m_source;
};

#endif // IMAGE_LOADER_H
