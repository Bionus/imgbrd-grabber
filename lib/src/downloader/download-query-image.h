#ifndef DOWNLOAD_QUERY_IMAGE_H
#define DOWNLOAD_QUERY_IMAGE_H

#include <QJsonObject>
#include <QMap>
#include <QMetaType>
#include <QSettings>
#include <QString>


class Image;
class Site;

class DownloadQueryImage
{
	public:
		// Constructors
		DownloadQueryImage() = default;
		DownloadQueryImage(QSettings *settings, const Image &img, Site *site);
		DownloadQueryImage(const Image &img, Site *site, QString filename, QString path);
		DownloadQueryImage(int id, const QString &md5, const QString &rating, const QString &tags, const QString &fileUrl, const QString &date, Site *site, QString filename, QString path);

		// Serialization
		QString toString(const QString &separator) const;
		void write(QJsonObject &json) const;
		bool read(const QJsonObject &json, const QMap<QString, Site *> &sites);

		// Public members
		Site *site;
		QString filename;
		QString path;
		QMap<QString, QString> values;

	private:
		void initFromImage(const Image &img);
		void initFromData(int id, const QString &md5, const QString &rating, const QString &tags, const QString &fileUrl, const QString &date);
};

bool operator==(const DownloadQueryImage &lhs, const DownloadQueryImage &rhs);
bool operator!=(const DownloadQueryImage &lhs, const DownloadQueryImage &rhs);

Q_DECLARE_METATYPE(DownloadQueryImage)

#endif // DOWNLOAD_QUERY_IMAGE_H
