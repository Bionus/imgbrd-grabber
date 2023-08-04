#ifndef API_ENDPOINT_H
#define API_ENDPOINT_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include "api.h"


class Page;

class ApiEndpoint
{
	public:
		virtual QString name() const = 0;
		virtual QStringList inputs() const = 0;
		virtual bool parseErrors() const = 0;

		virtual PageUrl url(const QMap<QString, QVariant> &query, int page, int limit, const PageInformation &lastPage, Site *site) const = 0;
		virtual ParsedPage parse(Page *parentPage, const QString &source, int statusCode, int first) const = 0;
};

#endif // API_ENDPOINT_H
