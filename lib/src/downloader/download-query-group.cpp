#include "download-query-group.h"


DownloadQueryGroup::DownloadQueryGroup()
{ }

DownloadQueryGroup::DownloadQueryGroup(QSettings *settings, QString tags, int page, int perpage, int total, QString site, QString unk)
	: tags(tags), page(page), perpage(perpage), total(total), getBlacklisted(getBlacklisted), site(site), unk(unk)
{
	getBlacklisted = settings->value("downloadblacklist").toBool();
	filename = settings->value("Save/filename").toString();
	path = settings->value("Save/path").toString();
}

DownloadQueryGroup::DownloadQueryGroup(QString tags, int page, int perpage, int total, bool getBlacklisted, QString site, QString filename, QString path, QString unk)
	: tags(tags), page(page), perpage(perpage), total(total), getBlacklisted(getBlacklisted), site(site), filename(filename), path(path), unk(unk)
{ }
