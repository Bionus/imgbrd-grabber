#include "downloader/download-query-group.h"
#include <QJsonArray>
#include "models/site.h"


DownloadQueryGroup::DownloadQueryGroup(QSettings *settings, QString tags, int page, int perPage, int total, QStringList postFiltering, Site *site, QString unk)
	: DownloadQuery(site), tags(std::move(tags)), page(page), perpage(perPage), total(total), postFiltering(std::move(postFiltering)), unk(std::move(unk))
{
	getBlacklisted = settings->value("downloadblacklist").toBool();
	filename = settings->value("Save/filename").toString();
	path = settings->value("Save/path").toString();
}

DownloadQueryGroup::DownloadQueryGroup(QString tags, int page, int perPage, int total, QStringList postFiltering, bool getBlacklisted, Site *site, const QString &filename, const QString &path, QString unk)
	: DownloadQuery(site, filename, path), tags(std::move(tags)), page(page), perpage(perPage), total(total), postFiltering(std::move(postFiltering)), getBlacklisted(getBlacklisted), unk(std::move(unk))
{ }


void DownloadQueryGroup::write(QJsonObject &json) const
{
	json["tags"] = QJsonArray::fromStringList(tags.split(' ', QString::SkipEmptyParts));
	json["page"] = page;
	json["perpage"] = perpage;
	json["total"] = total;
	json["postFiltering"] = QJsonArray::fromStringList(postFiltering);
	json["getBlacklisted"] = getBlacklisted;

	json["site"] = site->url();
	json["filename"] = QString(filename).replace("\n", "\\n");
	json["path"] = path;
}

bool DownloadQueryGroup::read(const QJsonObject &json, const QMap<QString, Site*> &sites)
{
	QJsonArray jsonTags = json["tags"].toArray();
	QStringList tgs;
	tgs.reserve(jsonTags.count());
	for (auto tag : jsonTags)
		tgs.append(tag.toString());

	tags = tgs.join(' ');
	page = json["page"].toInt();
	perpage = json["perpage"].toInt();
	total = json["total"].toInt();
	getBlacklisted = json["getBlacklisted"].toBool();

	filename = json["filename"].toString().replace("\\n", "\n");
	path = json["path"].toString();

	// Post filtering
	postFiltering.clear();
	QJsonArray jsonPostFilters = json["postFiltering"].toArray();
	for (auto tag : jsonPostFilters)
		postFiltering.append(tag.toString());

	// Get site
	const QString siteName = json["site"].toString();
	if (!sites.contains(siteName))
	{
		return false;
	}
	site = sites[siteName];

	// Validate values
	if (page < 1 || perpage < 1 || total < 1)
	{
		return false;
	}

	return true;
}


bool operator==(const DownloadQueryGroup& lhs, const DownloadQueryGroup& rhs)
{
	return lhs.tags == rhs.tags
			&& lhs.page == rhs.page
			&& lhs.perpage == rhs.perpage
			&& lhs.total == rhs.total
			&& lhs.getBlacklisted == rhs.getBlacklisted
			&& lhs.site == rhs.site
			&& lhs.filename == rhs.filename
			&& lhs.path == rhs.path;
}

bool operator!=(const DownloadQueryGroup& lhs, const DownloadQueryGroup& rhs)
{
	return !(lhs == rhs);
}
