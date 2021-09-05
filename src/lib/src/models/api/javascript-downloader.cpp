#include "models/api/javascript-downloader.h"
#include <QJSValueIterator>
#include "js-helpers.h"
#include "logger.h"


JavascriptDownloader::JavascriptDownloader(QString name, const QJSValue &handler)
	: m_name(std::move(name)), m_handler(handler)
{
	const auto &rxJs = m_handler.property("regexes");
	const QStringList rxStr = rxJs.isArray()
		? jsToStringList(rxJs)
		: QStringList(rxJs.toString());

	m_regularExpressions.reserve(rxStr.length());
	for (const auto &str : rxStr) {
		m_regularExpressions.append(QRegularExpression(str));
	}
}


QString JavascriptDownloader::getName() const
{
	return m_name;
}


bool JavascriptDownloader::canHandle(const QUrl &url) const
{
	const QString str = url.toString();
	return std::any_of(
		m_regularExpressions.constBegin(),
		m_regularExpressions.constEnd(),
		[&str](const QRegularExpression &rx) {
			return rx.match(str).hasMatch();
		}
	);
}

DownloadableUrl JavascriptDownloader::url(const QUrl &url) const
{
	DownloadableUrl ret;

	QJSValue urlFunction = m_handler.property("url");
	if (urlFunction.isUndefined()) {
		ret.url = url;
		return ret;
	}

	const QJSValue result = urlFunction.call(QList<QJSValue> { url.toString() });

	// Script errors and exceptions
	if (result.isError()) {
		const QString err = QStringLiteral("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString());
		ret.error = err;
		log(err, Logger::Error);
		return ret;
	}

	// Parse result
	if (result.isObject()) {
		if (result.hasProperty("error")) {
			ret.error = result.property("error").toString();
			return ret;
		}

		ret.url = result.property("url").toString();

		if (result.hasProperty("headers")) {
			const QJSValue headers = result.property("headers");
			QJSValueIterator headersIt(headers);
			while (headersIt.hasNext()) {
				headersIt.next();
				ret.headers[headersIt.name()] = headersIt.value().toString();
			}
		}
	} else {
		ret.url = result.toString();
	}

	return ret;
}

DownloadableData JavascriptDownloader::parse(const QString &source, int statusCode) const
{
	DownloadableData ret;

	QJSValue parseFunction = m_handler.property("parse");
	const QJSValue result = parseFunction.call(QList<QJSValue> { source, statusCode });

	// Script errors and exceptions
	if (result.isError()) {
		const QString err = QStringLiteral("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString());
		ret.error = err;
		log(err, Logger::Error);
		return ret;
	}

	if (result.hasProperty("error")) {
		ret.error = result.property("error").toString();
		return ret;
	}

	if (result.hasProperty("tokens")) {
		const QJSValue tokens = result.property("tokens");
		QJSValueIterator tokensIt(tokens);
		while (tokensIt.hasNext()) {
			tokensIt.next();
			ret.tokens[tokensIt.name()] = tokensIt.value().toString();
		}
	}

	if (result.hasProperty("files")) {
		const QJSValue files = result.property("files");
		const quint32 length = files.property("length").toUInt();
		ret.files.reserve(length);
		for (quint32 i = 0; i < length; ++i) {
			const QJSValue file = files.property(i);
			ret.files.append(parseDownloadableFile(file));
		}
	}

	return ret;
}

DownloadableFile JavascriptDownloader::parseDownloadableFile(const QJSValue &jsVal) const
{
	DownloadableFile ret;
	getProperty(jsVal, "url", ret.url);
	getProperty(jsVal, "width", ret.width);
	getProperty(jsVal, "height", ret.height);
	getProperty(jsVal, "filesize", ret.filesize);
	getProperty(jsVal, "ext", ret.ext);
	return ret;
}
