#include "batch-download-image.h"
#include "downloader/download-query-group.h"
#include "downloader/download-query-image.h"


const DownloadQuery *BatchDownloadImage::query() const
{
	if (queryGroup != nullptr)
		return queryGroup;

	return queryImage;
}

int BatchDownloadImage::siteId(const QList<DownloadQueryGroup> &groups) const
{
	if (queryGroup != nullptr)
	{
		const int index = groups.indexOf(*queryGroup);
		if (index >= 0)
			return index + 1;
	}

	return -1;
}


bool operator==(const BatchDownloadImage &lhs, const BatchDownloadImage &rhs)
{
	return lhs.image == rhs.image;
}
