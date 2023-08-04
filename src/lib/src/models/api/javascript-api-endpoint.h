#ifndef JAVASCRIPT_API_ENDPOINT_H
#define JAVASCRIPT_API_ENDPOINT_H

#include "models/api/api-endpoint.h"
#include <QJSValue>


class JavascriptApi;
class Page;
class QJSEngine;
class QMutex;
class Site;
class Tag;

class JavascriptApiEndpoint : public QObject, public ApiEndpoint
{
	Q_OBJECT

	public:
		JavascriptApiEndpoint(JavascriptApi *api, QJSEngine *engine, QJSValue endpoint, QObject *parent = nullptr);

		QString name() const override;
		QStringList inputs() const override;
		bool parseErrors() const override;

		PageUrl url(const QMap<QString, QVariant> &query, int page, int limit, const PageInformation &lastPage, Site *site) const override;
		ParsedPage parse(Page *parentPage, const QString &source, int statusCode, int first) const override;

	private:
		JavascriptApi *m_api; // TODO: get rid of this
		QJSEngine *m_engine;
		QJSValue m_endpoint;
};

#endif // JAVASCRIPT_API_ENDPOINT_H
