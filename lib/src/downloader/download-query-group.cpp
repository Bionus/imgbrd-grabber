#include "download-query-group.h"
#include <QJsonArray>
#include "models/site.h"


DownloadQueryGroup::DownloadQueryGroup()
{ }

DownloadQueryGroup::DownloadQueryGroup(QSettings *settings, QString tags, int page, int perPage, int total, QStringList postFiltering, Site *site, QString unk)
	: tags(tags), page(page), perpage(perPage), total(total), postFiltering(postFiltering), site(site), unk(unk)
{
	getBlacklisted = settings->value("downloadblacklist").toBool();
	filename = settings->value("Save/filename").toString();
	path = settings->value("Save/path").toString();
}

DownloadQueryGroup::DownloadQueryGroup(QString tags, int page, int perPage, int total, QStringList postFiltering, bool blacklisted, Site *site, QString filename, QString path, QString unk)
	: tags(tags), page(page), perpage(perPage), total(total), postFiltering(postFiltering), getBlacklisted(blacklisted), site(site), filename(filename), path(path), unk(unk)
{ }


QString DownloadQueryGroup::toString(QString separator) const
{
	return tags + separator +
			QString::number(page) + separator +
			QString::number(perpage) + separator +
			QString::number(total) + separator +
			(getBlacklisted ? "true" : "false") + separator +
			site->url() + separator +
			QString(filename).replace("\n", "\\n") + separator +
			path + separator;
}

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
	QStringList tgs;
	QJsonArray jsonTags = json["tags"].toArray();
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
	QString siteName = json["site"].toString();
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
