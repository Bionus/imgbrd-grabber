#include "downloader/download-query-image.h"
#include <QJsonArray>
#include "tags/tag.h"
#include "models/image.h"
#include "models/site.h"


DownloadQueryImage::DownloadQueryImage(QSettings *settings, const Image &img, Site *site)
	: site(site)
{
	filename = settings->value("Save/filename").toString();
	path = settings->value("Save/path").toString();

	initFromImage(img);
}

DownloadQueryImage::DownloadQueryImage(const Image &img, Site *site, QString filename, QString path)
	: site(site), filename(filename), path(path)
{
	initFromImage(img);
}

DownloadQueryImage::DownloadQueryImage(int id, const QString &md5, const QString &rating, const QString &tags, const QString &fileUrl, const QString &date, Site *site, QString filename, QString path)
	: site(site), filename(filename), path(path)
{
	initFromData(id, md5, rating, tags, fileUrl, date);
}

void DownloadQueryImage::initFromImage(const Image &img)
{
	QStringList tags;
	for (const Tag &tag : img.tags())
		tags.append(tag.text());

	initFromData(img.id(), img.md5(), img.rating(), tags.join(" "), img.fileUrl().toString(), img.createdAt().toString(Qt::ISODate));
}

void DownloadQueryImage::initFromData(int id, const QString &md5, const QString &rating, const QString &tags, const QString &fileUrl, const QString &date)
{
	values["filename"] = filename;
	values["path"] = path;
	values["site"] = site->name();

	values["id"] = QString::number(id);
	values["md5"] = md5;
	values["rating"] = rating;
	values["tags"] = tags;
	values["date"] = date;
	values["file_url"] = fileUrl;

	QString t = site->contains("DefaultTag") ? site->value("DefaultTag") : "";
	values["page_url"] = site->value("Urls/Html/Post");
	values["page_url"].replace("{tags}", t);
	values["page_url"].replace("{id}", QString::number(id));
}


QString DownloadQueryImage::toString(const QString &separator) const
{
	return values["id"] + separator +
			values["md5"] + separator +
			values["rating"] + separator +
			values["tags"] + separator +
			values["file_url"] + separator +
			values["date"] + separator +
			site->url() + separator +
			filename + separator +
			path;
}

void DownloadQueryImage::write(QJsonObject &json) const
{
	json["id"] = values["id"].toInt();
	json["md5"] = values["md5"];
	json["rating"] = values["rating"];
	json["tags"] = QJsonArray::fromStringList(values["tags"].split(' ', QString::SkipEmptyParts));
	json["file_url"] = values["file_url"];
	json["date"] = values["date"];

	json["site"] = site->url();
	json["filename"] = QString(filename).replace("\n", "\\n");
	json["path"] = path;
}

bool DownloadQueryImage::read(const QJsonObject &json, const QMap<QString, Site *> &sites)
{
	QStringList tags;
	QJsonArray jsonTags = json["tags"].toArray();
	for (auto tag : jsonTags)
		tags.append(tag.toString());

	values["id"] = QString::number(json["id"].toInt());
	values["md5"] = json["md5"].toString();
	values["rating"] = json["rating"].toString();
	values["tags"] = tags.join(' ');
	values["file_url"] = json["file_url"].toString();
	values["date"] = json["date"].toString();

	filename = json["filename"].toString().replace("\\n", "\n");
	path = json["path"].toString();

	// Get site
	QString siteName = json["site"].toString();
	if (!sites.contains(siteName))
	{
		return false;
	}
	site = sites[siteName];

	return true;
}


bool operator==(const DownloadQueryImage &lhs, const DownloadQueryImage &rhs)
{
	return lhs.values == rhs.values
			&& lhs.site == rhs.site
			&& lhs.filename == rhs.filename
			&& lhs.path == rhs.path;
}

bool operator!=(const DownloadQueryImage &lhs, const DownloadQueryImage &rhs)
{
	return !(lhs == rhs);
}
