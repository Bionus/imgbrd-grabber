#include "gallery-search-loader.h"


GallerySearchLoader::GallerySearchLoader(QObject *parent)
	: SearchLoader(parent)
{}


const QSharedPointer<Image> &GallerySearchLoader::gallery() const
{
	return m_gallery;
}

void GallerySearchLoader::setGallery(const QSharedPointer<Image> &gallery)
{
	m_gallery = gallery;
	emit galleryChanged();
}


void GallerySearchLoader::load()
{
	if (!m_gallery.isNull()) {
		search(m_gallery);
	}
}
