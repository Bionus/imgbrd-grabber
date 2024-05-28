#ifndef GALLERY_SEARCH_LOADER_H
#define GALLERY_SEARCH_LOADER_H

#include "search-loader.h"
#include <QSharedPointer>

Q_MOC_INCLUDE("models/image.h")


class Image;

class GallerySearchLoader : public SearchLoader
{
	Q_OBJECT

	Q_PROPERTY(QSharedPointer<Image> gallery READ gallery WRITE setGallery NOTIFY galleryChanged)

	public:
		explicit GallerySearchLoader(QObject *parent = nullptr);

		const QSharedPointer<Image> &gallery() const;
		void setGallery(const QSharedPointer<Image> &gallery);

	public slots:
		void load() override;

	signals:
		void galleryChanged();

	private:
		QSharedPointer<Image> m_gallery;
};

#endif // GALLERY_SEARCH_LOADER_H
