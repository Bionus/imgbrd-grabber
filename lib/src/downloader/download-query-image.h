#ifndef DOWNLOAD_QUERY_IMAGE_H
#define DOWNLOAD_QUERY_IMAGE_H

#include <QMetaType>
#include <QString>
#include <QSettings>
#include <QMap>
#include <QJsonObject>


class Image;
class Site;

class DownloadQueryImage
{
	public:
		// Constructors
		DownloadQueryImage();
		DownloadQueryImage(QSettings *settings, QSharedPointer<Image> img, Site *site);
		DownloadQueryImage(QSharedPointer<Image> img, Site *site, QString filename, QString path);
		DownloadQueryImage(int id, QString md5, QString rating, QString tags, QString fileUrl, QString date, Site *site, QString filename, QString path);

		// Serialization
		QString toString(QString separator) const;
		void write(QJsonObject &json) const;
		bool read(const QJsonObject &json, QMap<QString, Site *> &sites);

		// Public members
		Site *site;
		QString filename;
		QString path;
		QMap<QString, QString> values;

	private:
		void initFromImage(QSharedPointer<Image> img);
		void initFromData(int id, QString md5, QString rating, QString tags, QString fileUrl, QString date);
};

bool operator==(const DownloadQueryImage& lhs, const DownloadQueryImage& rhs);
bool operator!=(const DownloadQueryImage& lhs, const DownloadQueryImage& rhs);

Q_DECLARE_METATYPE(DownloadQueryImage)

#endif // DOWNLOAD_QUERY_IMAGE_H
