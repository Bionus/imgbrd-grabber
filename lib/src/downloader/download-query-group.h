#ifndef DOWNLOAD_QUERY_GROUP_H
#define DOWNLOAD_QUERY_GROUP_H

#include <QMetaType>
#include <QString>
#include <QSettings>
#include <QJsonObject>


class Site;

class DownloadQueryGroup
{
	public:
		// Constructors
		DownloadQueryGroup() = default;
		DownloadQueryGroup(QSettings *settings, QString tags, int page, int perPage, int total, QStringList postFiltering, Site *site, QString unk = "");
		DownloadQueryGroup(QString tags, int page, int perPage, int total, QStringList postFiltering, bool getBlacklisted, Site *site, QString filename, QString path, QString unk = "");

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
