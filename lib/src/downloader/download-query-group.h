#ifndef DOWNLOAD_QUERY_GROUP_H
#define DOWNLOAD_QUERY_GROUP_H

#include <QJsonObject>
#include <QSettings>
#include <QString>
#include "downloader/download-query.h"


class Site;

class DownloadQueryGroup : public DownloadQuery
{
	public:
		// Constructors
		DownloadQueryGroup() = default;
		explicit DownloadQueryGroup(QSettings *settings, const QString &tags, int page, int perPage, int total, const QStringList &postFiltering, Site *site, const QString &unk = QString());
		explicit DownloadQueryGroup(const QString &tags, int page, int perPage, int total, const QStringList &postFiltering, bool getBlacklisted, Site *site, const QString &filename, const QString &path, const QString &unk = QString());

		// Serialization
		QString toString(const QString &separator) const override;
		void write(QJsonObject &json) const override;
		bool read(const QJsonObject &json, const QMap<QString, Site*> &sites) override;

		// Public members
		QString tags;
		int page;
		int perpage;
		int total;
		QStringList postFiltering;
		bool getBlacklisted;
		QString unk;
};

bool operator==(const DownloadQueryGroup& lhs, const DownloadQueryGroup& rhs);
bool operator!=(const DownloadQueryGroup& lhs, const DownloadQueryGroup& rhs);

Q_DECLARE_METATYPE(DownloadQueryGroup)

#endif // DOWNLOAD_QUERY_GROUP_H
