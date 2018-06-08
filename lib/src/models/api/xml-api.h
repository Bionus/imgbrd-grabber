#ifndef XML_API_H
#define XML_API_H

#include "models/api/api.h"


class XmlApi : public Api
{
	Q_OBJECT

	public:
		explicit XmlApi(const QMap<QString, QString> &data);
		ParsedPage parsePage(Page *parentPage, const QString &source, int first, int limit) const override;
		ParsedTags parseTags(const QString &source, Site *site) const override;
};

#endif // XML_API_H
