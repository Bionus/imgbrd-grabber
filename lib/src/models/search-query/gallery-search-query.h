#ifndef GALLERY_SEARCH_QUERY_H
#define GALLERY_SEARCH_QUERY_H

#include <QSharedPointer>
#include "models/search-query/search-query.h"


class Image;

class GallerySearchQuery : public SearchQuery
{
	public:
		explicit GallerySearchQuery(QSharedPointer<Image> gallery);
		QSharedPointer<Image> gallery() const;

	private:
		QSharedPointer<Image> m_gallery;
};

#endif // GALLERY_SEARCH_QUERY_H
