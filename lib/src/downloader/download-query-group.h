#ifndef DOWNLOAD_QUERY_GROUP_H
#define DOWNLOAD_QUERY_GROUP_H

#include <QJsonObject>
#include <QMetaType>
#include <QSettings>
#include <QString>


class Site;

class DownloadQueryGroup
{
	public:
		// Constructors
		DownloadQueryGroup() = default;
		DownloadQueryGroup(QSettings *settings, const QString &tags, int page, int perPage, int total, const QStringList &postFiltering, Site *site, const QString &unk = "");
		DownloadQueryGroup(const QString &tags, int page, int perPage, int total, const QStringList &postFiltering, bool getBlacklisted, Site *site, const QString &filename, const QString &path, const QString &unk = "");

		// Serialization
		QString toString(const QString &separator) const;
		void write(QJsonObject &json) const;
		bool read(const QJsonObject &json, const QMap<QString, Site*> &sites);

		// Public members
		QString tags;
		int page;
		int perpage;
		int total;
		QStringList postFiltering;
		bool getBlacklisted;
		Site *site;
		QString filename;
		QString path;
		QString unk;
};

bool operator==(const DownloadQueryGroup& lhs, const DownloadQueryGroup& rhs);
bool operator!=(const DownloadQueryGroup& lhs, const DownloadQueryGroup& rhs);

Q_DECLARE_METATYPE(DownloadQueryGroup)

#endif // DOWNLOAD_QUERY_GROUP_H
