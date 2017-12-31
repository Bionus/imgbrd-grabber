#ifndef HTML_API_H
#define HTML_API_H

#include "models/api/api.h"


class HtmlApi : public Api
{
	Q_OBJECT

	public:
		explicit HtmlApi(const QMap<QString, QString> &data);
		ParsedPage parsePage(Page *parentPage, const QString &source, int first) const override;
};

#endif // HTML_API_H
