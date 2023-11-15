#include "javascript-api-endpoint.h"
#include <QJSEngine>
#include <QList>
#include <utility>
#include "javascript-api.h"
#include "js-helpers.h"
#include "models/page.h"
#include "models/site.h"


JavascriptApiEndpoint::JavascriptApiEndpoint(JavascriptApi *api, QJSEngine *engine, QJSValue endpoint, QObject *parent)
	: QObject(parent), m_api(api), m_engine(engine), m_endpoint(std::move(endpoint))
{}


QString JavascriptApiEndpoint::name() const
{
	return getPropertyOr(m_endpoint, "name", QString());
}

QStringList JavascriptApiEndpoint::inputs() const
{
	return QStringList();
}

bool JavascriptApiEndpoint::parseErrors() const
{
	return getPropertyOr(m_endpoint, "parseErrors", false);
}


PageUrl JavascriptApiEndpoint::url(const QMap<QString, QVariant> &query, int page, int limit, const PageInformation &lastPage, Site *site) const
{
	PageUrl ret;

	QJSValue urlFunction = m_endpoint.property("url");

	QJSValue jsQuery = m_engine->newObject();
	for (auto it = query.constBegin(); it != query.constEnd(); ++it) {
		jsQuery.setProperty(it.key(), m_engine->toScriptValue(it.value()));
	}

	QJSValue opts = m_engine->newObject();
	opts.setProperty("page", page);
	opts.setProperty("limit", limit);
	opts.setProperty("baseUrl", site->baseUrl());
	opts.setProperty("loggedIn", site->isLoggedIn(false, true));

	QJSValue previous = QJSValue::UndefinedValue;
	if (lastPage.page > 0) {
		previous = m_engine->newObject();
		previous.setProperty("page", lastPage.page);
		previous.setProperty("minIdM1", QString::number(lastPage.minId - 1));
		previous.setProperty("minId", QString::number(lastPage.minId));
		previous.setProperty("maxId", QString::number(lastPage.maxId));
		previous.setProperty("maxIdP1", QString::number(lastPage.maxId + 1));
		previous.setProperty("minDate", lastPage.minDate);
		previous.setProperty("maxDate", lastPage.maxDate);
	}

	const QJSValue result = urlFunction.call(QList<QJSValue> { jsQuery, opts, previous });
	m_api->fillUrlObject(result, site, ret);

	return ret;
}

ParsedPage JavascriptApiEndpoint::parse(Page *parentPage, const QString &source, int statusCode, int first) const
{
	ParsedPage ret;

	Site *site = parentPage->site();
	QJSValue parseFunction = m_endpoint.property("parse");
	const QJSValue &results = parseFunction.call(QList<QJSValue> { source, statusCode });

	// Script errors and exceptions
	if (results.isError()) {
		ret.error = QStringLiteral("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
		return ret;
	}

	if (results.hasProperty("error")) {
		ret.error = results.property("error").toString();
	}

	if (results.hasProperty("tags")) {
		ret.tags = m_api->makeTags(results.property("tags"), site);
	}

	if (results.hasProperty("images")) {
		const QJSValue images = results.property("images");
		const quint32 length = images.property("length").toUInt();
		for (quint32 i = 0; i < length; ++i) {
			auto img = m_api->makeImage(images.property(i), site, parentPage, i, first);
			if (!img.isNull()) {
				ret.images.append(img);
			}
		}
	}

	// Basic properties
	getProperty(results, "imageCount", ret.imageCount);
	getProperty(results, "pageCount", ret.pageCount);
	getProperty(results, "urlNextPage", ret.urlNextPage);
	getProperty(results, "urlPrevPage", ret.urlPrevPage);
	if (results.hasProperty("wiki") && results.property("wiki").isString()) {
		ret.wiki = results.property("wiki").toString();
		ret.wiki = ret.wiki.replace("href=\"/", "href=\"" + site->baseUrl() + "/");
	}

	return ret;
}
