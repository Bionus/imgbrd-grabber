#include "downloader/download-query-image.h"
#include <QJsonArray>
#include <QSettings>
#include "models/image.h"
#include "models/site.h"
#include "tags/tag.h"


DownloadQueryImage::DownloadQueryImage(QSettings *settings, const Image &img, Site *site)
	: DownloadQuery(site)
{
	filename = settings->value("Save/filename").toString();
	path = settings->value("Save/path").toString();

	initFromImage(img);
}

DownloadQueryImage::DownloadQueryImage(const Image &img, Site *site, const QString &filename, const QString &path)
	: DownloadQuery(site, filename, path)
{
	initFromImage(img);
}

DownloadQueryImage::DownloadQueryImage(qulonglong id, const QString &md5, const QString &rating, const QString &tags, const QString &fileUrl, const QString &date, Site *site, const QString &filename, const QString &path, const QStringList &search)
	: DownloadQuery(site, filename, path)
{
	initFromData(id, md5, rating, tags, fileUrl, date, search);
}

void DownloadQueryImage::initFromImage(const Image &img)
{
	const QList<Tag> &imgTags = img.tags();

	QStringList tags;
	tags.reserve(imgTags.count());
	for (const Tag &tag : imgTags)
		tags.append(tag.text());

	const QMap<QString, Token> &tokens = img.tokens(nullptr);
	initFromData(
		tokens["id"].value().toULongLong(),
		tokens["md5"].value().toString(),
		tokens["rating"].value().toString(),
		tags.join(" "),
		tokens["url_original"].value().toString(),
		tokens["date"].value().toDateTime().toString(Qt::ISODate),
		tokens["search"].value().toString().split(' ')
	);
}

void DownloadQueryImage::initFromData(qulonglong id, const QString &md5, const QString &rating, const QString &tags, const QString &fileUrl, const QString &date, const QStringList &search)
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
	values["search"] = search.join(' ');
}


void DownloadQueryImage::write(QJsonObject &json) const
{
	json["id"] = values["id"].toInt();
	json["md5"] = values["md5"];
	json["rating"] = values["rating"];
	json["tags"] = QJsonArray::fromStringList(values["tags"].split(' ', QString::SkipEmptyParts));
	json["file_url"] = values["file_url"];
	json["date"] = values["date"];
	json["search"] = values["search"];

	json["site"] = site->url();
	json["filename"] = QString(filename).replace("\n", "\\n");
	json["path"] = path;
}

bool DownloadQueryImage::read(const QJsonObject &json, const QMap<QString, Site *> &sites)
{
	QJsonArray jsonTags = json["tags"].toArray();
	QStringList tags;
	tags.reserve(jsonTags.count());
	for (auto tag : jsonTags)
		tags.append(tag.toString());

	values["id"] = QString::number(json["id"].toInt());
	values["md5"] = json["md5"].toString();
	values["rating"] = json["rating"].toString();
	values["tags"] = tags.join(' ');
	values["file_url"] = json["file_url"].toString();
	values["date"] = json["date"].toString();
	values["search"] = json["search"].toString();

	filename = json["filename"].toString().replace("\\n", "\n");
	path = json["path"].toString();

	// Get site
	const QString siteName = json["site"].toString();
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
