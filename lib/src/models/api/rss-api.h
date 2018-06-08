#ifndef RSS_API_H
#define RSS_API_H

#include "models/api/api.h"


class RssApi : public Api
{
	Q_OBJECT

	public:
		explicit RssApi(const QMap<QString, QString> &data);
		ParsedPage parsePage(Page *parentPage, const QString &source, int first, int limit) const override;
		ParsedTags parseTags(const QString &source, Site *site) const override;
		bool canLoadDetails() const override;
};

#endif // RSS_API_H
