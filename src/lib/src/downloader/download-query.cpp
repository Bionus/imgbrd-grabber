#include "downloader/download-query.h"
#include <utility>


DownloadQuery::DownloadQuery(Site *site)
	: site(site)
{}

DownloadQuery::DownloadQuery(Site *site, QString filename, QString path)
	: site(site), filename(std::move(filename)), path(std::move(path))
{}
