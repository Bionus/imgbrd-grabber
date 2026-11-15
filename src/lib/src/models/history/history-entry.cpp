#include "history-entry.h"
#include <QJsonArray>
#include <QJsonObject>
#include "models/profile.h"
#include "models/site.h"


HistoryEntry HistoryEntry::fromJson(const QJsonObject &json, Profile *profile)
{
	const QMap<QString, Site*> &sitesMapping = profile->getSites();

	// Query
	SearchQuery query;
	query.read(json["query"].toObject(), profile);

	// Sites
	QList<Site*> sites;
	QJsonArray sitesJson = json["sites"].toArray();
	sites.reserve(sitesJson.count());
	for (const auto &siteJson : sitesJson) {
		const QString siteUrl = siteJson.toString();
		if (sitesMapping.contains(siteUrl)) {
			sites.append(sitesMapping.value(siteUrl));
		}
	}

	return {
		query,
		sites,
		QDateTime::fromString(json["date"].toString(), Qt::ISODate),
	};
}

void HistoryEntry::toJson(QJsonObject &json) const
{
	// Query
	QJsonObject queryJson;
	query.write(queryJson);

	// Sites
	QJsonArray sitesJson;
	for (const Site *site : sites) {
		sitesJson.append(site->url());
	}

	json["query"] = queryJson;
	json["sites"] = sitesJson;
	json["date"] = date.toString(Qt::ISODate);
}
