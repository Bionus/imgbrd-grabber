#include "download-query-group.h"
#include <QJsonArray>


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


QString DownloadQueryGroup::toString(QString separator) const
{
	return tags + separator +
			QString::number(page) + separator +
			QString::number(perpage) + separator +
			QString::number(total) + separator +
			(getBlacklisted ? "true" : "false") + separator +
			site + separator +
			QString(filename).replace("\n", "\\n") + separator +
			path + separator;
}

void DownloadQueryGroup::write(QJsonObject &json) const
{
	json["tags"] = QJsonArray::fromStringList(tags.split(' '));
	json["page"] = page;
	json["perpage"] = perpage;
	json["total"] = total;
	json["getBlacklisted"] = getBlacklisted;

	json["site"] = site;
	json["filename"] = QString(filename).replace("\n", "\\n");
	json["path"] = path;
}

bool DownloadQueryGroup::read(const QJsonObject &json, QMap<QString, Site*> &sites)
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

	site = json["site"].toString();
	filename = json["filename"].toString().replace("\\n", "\n");
	path = json["path"].toString();

	// Validate values
	if (page < 1 || perpage < 1 || total < 1 || !sites.contains(site))
	{
		return false;
	}

	return true;
}
