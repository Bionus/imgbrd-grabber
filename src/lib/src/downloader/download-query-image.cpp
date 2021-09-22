#include "downloader/download-query-image.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSettings>
#include <utility>
#include "logger.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"
#include "tags/tag.h"


DownloadQueryImage::DownloadQueryImage(QSettings *settings, QSharedPointer<Image> img, Site *site)
	: DownloadQuery(site), image(std::move(img))
{
	filename = settings->value("Save/filename").toString();
	path = settings->value("Save/path").toString();
}

DownloadQueryImage::DownloadQueryImage(QSharedPointer<Image> img, Site *site, const QString &filename, const QString &path)
	: DownloadQuery(site, filename, path), image(std::move(img))
{}


void DownloadQueryImage::write(QJsonObject &json) const
{
	json["site"] = site->url();
	json["filename"] = QString(filename).replace("\\n", "\\\\n").replace("\n", "\\n");
	json["path"] = path;

	QJsonObject jsonImage;
	image->write(jsonImage);
	json["image"] = jsonImage;
}

bool DownloadQueryImage::read(const QJsonObject &json, Profile *profile)
{
	const QMap<QString, Site*> &sites = profile->getSites();

	const QString siteName = json["site"].toString();
	if (!sites.contains(siteName)) {
		log(QStringLiteral("Unknown site: %1").arg(siteName), Logger::Warning);
		return false;
	}

	auto img = new Image(profile);
	if (img->read(json["image"].toObject(), sites)) {
		image = QSharedPointer<Image>(img);
	} else {
		img->deleteLater();
		return false;
	}

	site = sites[siteName];
	static const QRegularExpression nlExpr("(?<=^|[^\\\\])\\\\n");
	filename = json["filename"].toString().replace(nlExpr, "\n").replace("\\\\n", "\\n");
	path = json["path"].toString();

	return true;
}


bool operator==(const DownloadQueryImage &lhs, const DownloadQueryImage &rhs)
{
	return lhs.image == rhs.image
		&& lhs.site == rhs.site
		&& lhs.filename == rhs.filename
		&& lhs.path == rhs.path;
}

bool operator!=(const DownloadQueryImage &lhs, const DownloadQueryImage &rhs)
{
	return !(lhs == rhs);
}
