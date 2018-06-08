#include "downloader/download-query.h"


DownloadQuery::DownloadQuery(Site *site)
	: site(site)
{}

DownloadQuery::DownloadQuery(Site *site, const QString &filename, const QString &path)
	: site(site), filename(filename), path(path)
{}
