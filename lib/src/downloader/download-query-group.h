#ifndef DOWNLOAD_QUERY_GROUP_H
#define DOWNLOAD_QUERY_GROUP_H

#include <QMetaType>
#include <QString>
#include <QStringList>
#include "downloader/download-query.h"
#include "models/search-query/search-query.h"


class Profile;
class QJsonObject;
class QSettings;
class Site;

class DownloadQueryGroup : public DownloadQuery
{
	public:
		// Constructors
		DownloadQueryGroup() = default;
		explicit DownloadQueryGroup(QSettings *settings, SearchQuery query, int page, int perPage, int total, QStringList postFiltering, Site *site);
		explicit DownloadQueryGroup(SearchQuery query, int page, int perPage, int total, QStringList postFiltering, bool getBlacklisted, Site *site, const QString &filename, const QString &path);

		// Serialization
		void write(QJsonObject &json) const override;
		bool read(const QJsonObject &json, Profile *profile) override;

		// Public members
		SearchQuery query;
		int page;
		int perpage;
		int total;
		QStringList postFiltering;
		bool getBlacklisted;
		bool galleriesCountAsOne = true;
		int progressVal = 0;
		bool progressFinished = false;
};

bool operator==(const DownloadQueryGroup &lhs, const DownloadQueryGroup &rhs);
bool operator!=(const DownloadQueryGroup &lhs, const DownloadQueryGroup &rhs);

Q_DECLARE_METATYPE(DownloadQueryGroup)

#endif // DOWNLOAD_QUERY_GROUP_H
