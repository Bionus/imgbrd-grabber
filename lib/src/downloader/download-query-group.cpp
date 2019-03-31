#include "downloader/download-query-group.h"
#include <QJsonArray>
#include <QSettings>
#include "models/site.h"


DownloadQueryGroup::DownloadQueryGroup(QSettings *settings, SearchQuery query, int page, int perPage, int total, QStringList postFiltering, Site *site)
	: DownloadQuery(site), query(std::move(query)), page(page), perpage(perPage), total(total), postFiltering(std::move(postFiltering))
{
	getBlacklisted = settings->value("downloadblacklist").toBool();
	filename = settings->value("Save/filename").toString();
	path = settings->value("Save/path").toString();
}

DownloadQueryGroup::DownloadQueryGroup(SearchQuery query, int page, int perPage, int total, QStringList postFiltering, bool getBlacklisted, Site *site, const QString &filename, const QString &path)
	: DownloadQuery(site, filename, path), query(std::move(query)), page(page), perpage(perPage), total(total), postFiltering(std::move(postFiltering)), getBlacklisted(getBlacklisted)
{}


void DownloadQueryGroup::write(QJsonObject &json) const
{
	QJsonObject jsonQuery;
	query.write(jsonQuery);
	json["query"] = jsonQuery;

	json["page"] = page;
	json["perpage"] = perpage;
	json["total"] = total;
	json["postFiltering"] = QJsonArray::fromStringList(postFiltering);
	json["getBlacklisted"] = getBlacklisted;
	json["galleriesCountAsOne"] = galleriesCountAsOne;

	json["site"] = site->url();
	json["filename"] = QString(filename).replace("\n", "\\n");
	json["path"] = path;

	json["progressVal"] = progressVal;
	json["progressFinished"] = progressFinished;
}

bool DownloadQueryGroup::read(const QJsonObject &json, const QMap<QString, Site*> &sites)
{
	query.read(json.contains("query") ? json["query"].toObject() : json, sites);

	page = json["page"].toInt();
	perpage = json["perpage"].toInt();
	total = json["total"].toInt();
	getBlacklisted = json["getBlacklisted"].toBool();
	galleriesCountAsOne = json["galleriesCountAsOne"].toBool();

	filename = json["filename"].toString().replace("\\n", "\n");
	path = json["path"].toString();

	progressVal = json["progressVal"].toInt();
	progressFinished = json["progressFinished"].toBool();

	// Post filtering
	postFiltering.clear();
	QJsonArray jsonPostFilters = json["postFiltering"].toArray();
	for (auto tag : jsonPostFilters) {
		postFiltering.append(tag.toString());
	}

	// Get site
	const QString siteName = json["site"].toString();
	if (!sites.contains(siteName)) {
		return false;
	}
	site = sites[siteName];

	// Validate values
	if (page < 1 || perpage < 1 || total < 1) {
		return false;
	}

	return true;
}


bool operator==(const DownloadQueryGroup &lhs, const DownloadQueryGroup &rhs)
{
	return lhs.query == rhs.query
		&& lhs.page == rhs.page
		&& lhs.perpage == rhs.perpage
		&& lhs.total == rhs.total
		&& lhs.getBlacklisted == rhs.getBlacklisted
		&& lhs.galleriesCountAsOne == rhs.galleriesCountAsOne
		&& lhs.site == rhs.site
		&& lhs.filename == rhs.filename
		&& lhs.path == rhs.path;
}

bool operator!=(const DownloadQueryGroup &lhs, const DownloadQueryGroup &rhs)
{
	return !(lhs == rhs);
}
