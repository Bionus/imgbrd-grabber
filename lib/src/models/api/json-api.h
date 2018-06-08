#ifndef JSON_API_H
#define JSON_API_H

#include "models/api/api.h"


class JsonApi : public Api
{
	Q_OBJECT

	public:
		explicit JsonApi(const QMap<QString, QString> &data);
		ParsedPage parsePage(Page *parentPage, const QString &source, int first, int limit) const override;
		ParsedTags parseTags(const QString &source, Site *site) const override;
};

#endif // JSON_API_H
