#ifndef DOWNLOAD_QUERY_IMAGE_H
#define DOWNLOAD_QUERY_IMAGE_H

#include <QJsonObject>
#include <QMap>
#include <QMetaType>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include "downloader/download-query.h"


class Image;
class QSettings;
class Site;

class DownloadQueryImage : public DownloadQuery
{
	public:
		// Constructors
		DownloadQueryImage() = default;
		explicit DownloadQueryImage(QSettings *settings, QSharedPointer<Image> img, Site *site);
		explicit DownloadQueryImage(QSharedPointer<Image> img, Site *site, const QString &filename, const QString &path);

		// Serialization
		void write(QJsonObject &json) const override;
		bool read(const QJsonObject &json, const QMap<QString, Site*> &sites) override;

		// Public members
		QSharedPointer<Image> image;
};

bool operator==(const DownloadQueryImage &lhs, const DownloadQueryImage &rhs);
bool operator!=(const DownloadQueryImage &lhs, const DownloadQueryImage &rhs);

Q_DECLARE_METATYPE(DownloadQueryImage)

#endif // DOWNLOAD_QUERY_IMAGE_H
