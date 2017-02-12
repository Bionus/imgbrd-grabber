#include "download-query-image.h"


DownloadQueryImage::DownloadQueryImage()
{ }

DownloadQueryImage::DownloadQueryImage(QSettings *settings, QSharedPointer<Image> img, Site *site)
	: site(site)
{
	filename = settings->value("Save/filename").toString();
	path = settings->value("Save/path").toString();

	initFromImage(img);
}

DownloadQueryImage::DownloadQueryImage(QSharedPointer<Image> img, Site *site, QString filename, QString path)
	: site(site), filename(filename), path(path)
{
	initFromImage(img);
}

DownloadQueryImage::DownloadQueryImage(int id, QString md5, QString rating, QString tags, QString fileUrl, QString date, Site *site, QString filename, QString path)
	: site(site), filename(filename), path(path)
{
	initFromData(id, md5, rating, tags, fileUrl, date);
}

void DownloadQueryImage::initFromImage(QSharedPointer<Image> img)
{
	QStringList tags;
	for (Tag tag : img->tags())
		tags.append(tag.text());

	initFromData(img->id(), img->md5(), img->rating(), tags.join(" "), img->fileUrl().toString(), img->createdAt().toString(Qt::ISODate));
}

void DownloadQueryImage::initFromData(int id, QString md5, QString rating, QString tags, QString fileUrl, QString date)
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


QString DownloadQueryImage::toString(QString separator) const
{
	return values["id"] + separator +
			values["md5"] + separator +
			values["rating"] + separator +
			values["tags"] + separator +
			values["file_url"] + separator +
			values["date"] + separator +
			site->name() + separator +
			filename + separator +
			path;
}
