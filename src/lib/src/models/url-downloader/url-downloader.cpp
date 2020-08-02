#include "url-downloader.h"
#include <QJSValue>
#include "js-helpers.h"
#include "logger.h"


UrlDownloader::UrlDownloader(QJSValue downloader, int index, QObject *parent)
	: QObject(parent), m_downloader(std::move(downloader)), m_index(index)
{
	m_name = downloader.property("name").toString();

	const QStringList regexes = jsToStringList(m_downloader.property("handlers").property(m_index).property("regexes"));
	m_regexes.reserve(regexes.count());
	for (const QString &regex : regexes) {
		m_regexes.append(QRegularExpression(regex));
	}
}

const QString &UrlDownloader::name() const
{
	return m_name;
}


bool UrlDownloader::canDownload(const QUrl &url) const
{
	for (const auto &regex : m_regexes) {
		if (regex.match(url.toString()).hasMatch()) {
			return true;
		}
	}
	return false;
}

UrlDownloaderUrl UrlDownloader::url(const QUrl &url) const
{
	UrlDownloaderUrl ret;

	QJSValue urlFunction = m_downloader.property("handlers").property(m_index).property("url");
	const QJSValue &result = urlFunction.call(QList<QJSValue> { url.toString() });

	// Script errors and exceptions
	if (result.isError()) {
		const QString err = QStringLiteral("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString());
		ret.error = err;
		log(err, Logger::Error);
		return ret;
	}

	// String return for basic URL
	if (!result.isObject()) {
		ret.url = result.toString();
		return ret;
	}

	// Error objects
	if (result.hasProperty("error")) {
		ret.error = result.property("error").toString();
		return ret;
	}

	// URL + headers object
	ret.url = result.property("url").toString();
	if (result.hasProperty("headers")) {
		const QJSValue &headers = result.property("headers");
		QJSValueIterator headersIt(headers);
		while (headersIt.hasNext()) {
			headersIt.next();
			ret.headers[headersIt.name()] = headersIt.value().toString();
		}
	}

	return ret;
}

UrlDownloaderResult UrlDownloader::parse(const QString &source, int statusCode) const
{
	UrlDownloaderResult ret;

	QJSValue parseFunction = m_downloader.property("handlers").property(m_index).property("parse");
	const QJSValue &result = parseFunction.call(QList<QJSValue> { source, statusCode });

	// Script errors and exceptions
	if (result.isError()) {
		const QString err = QStringLiteral("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString());
		ret.error = err;
		log(err, Logger::Error);
		return ret;
	}

	// Error objects
	if (result.hasProperty("error")) {
		ret.error = result.property("error").toString();
		return ret;
	}

	// Tokens
	if (result.hasProperty("tokens")) {
		const QJSValue &tokens = result.property("tokens");
		QJSValueIterator tokensIt(tokens);
		while (tokensIt.hasNext()) {
			tokensIt.next();
			ret.tokens[tokensIt.name()] = tokensIt.value().toVariant();
		}
	}

	// Files
	if (result.hasProperty("files")) {
		const QJSValue &files = result.property("files");
		const quint32 length = files.property("length").toUInt();
		for (quint32 i = 0; i < length; ++i) {
			const QJSValue &file = files.property(i);

			UrlDownloaderFile rFile;
			if (file.hasProperty("url") && !file.property("url").isUndefined()) {
				rFile.url = file.property("url").toString();
			}
			if (file.hasProperty("width") && !file.property("width").isUndefined()) {
				rFile.width = file.property("width").toInt();
			}
			if (file.hasProperty("height") && !file.property("height").isUndefined()) {
				rFile.height = file.property("height").toInt();
			}
			if (file.hasProperty("filesize") && !file.property("filesize").isUndefined()) {
				rFile.filesize = file.property("filesize").toInt();
			}
			if (file.hasProperty("ext") && !file.property("ext").isUndefined()) {
				rFile.ext = file.property("ext").toString();
			}

			ret.files.append(rFile);
		}
	}

	return ret;
}
