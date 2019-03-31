#include "models/search-query/gallery-search-query.h"


GallerySearchQuery::GallerySearchQuery(QSharedPointer<Image> gallery)
	: m_gallery(std::move(gallery))
{}

QSharedPointer<Image> GallerySearchQuery::gallery() const
{
	return m_gallery;
}
