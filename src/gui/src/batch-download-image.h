#ifndef BATCH_DOWNLOAD_IMAGE_H
#define BATCH_DOWNLOAD_IMAGE_H

#include <QSharedPointer>


class DownloadQuery;
class DownloadQueryGroup;
class DownloadQueryImage;
class Image;

class BatchDownloadImage
{
	public:
		const DownloadQuery *query() const;
		int siteId(const QList<DownloadQueryGroup> &groups) const;

		// Public members
		QSharedPointer<Image> image;
		const DownloadQueryGroup *queryGroup = nullptr;
		const DownloadQueryImage *queryImage = nullptr;
};

bool operator==(const BatchDownloadImage &lhs, const BatchDownloadImage &rhs);

#endif // BATCH_DOWNLOAD_IMAGE_H
