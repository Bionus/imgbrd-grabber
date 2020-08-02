#include "url-downloader-manager.h"
#include <QDir>
#include <QFile>
#include <QJSEngine>
#include <QJSValue>
#include "url-downloader.h"
#include "js-helpers.h"
#include "logger.h"


UrlDownloaderManager::UrlDownloaderManager(const QString &root, QObject *parent)
	: QObject(parent)
{
	m_engine = buildJsEngine(root + "/helper.js");

	const QStringList dirs = QDir(root).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString &dir : dirs) {
		load(root + "/" + dir + "/downloader.js");
	}
}

bool UrlDownloaderManager::load(const QString &file)
{
	QFile f(file);
	if (!f.exists() || !f.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;
	}

	const QString src = "(function() { var window = {}; " + f.readAll().replace("export var downloader = ", "return ") + " })()";
	f.close();

	QJSValue result = m_engine->evaluate(src, f.fileName());
	if (result.isError()) {
		log(QStringLiteral("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString()), Logger::Error);
		return false;
	}

	const quint32 length = result.property("handlers").property("length").toUInt();
	for (quint32 i = 0; i < length; ++i) {
		m_downloaders.append(new UrlDownloader(result, i, this));
	}

	return true;
}

UrlDownloader *UrlDownloaderManager::canDownload(const QUrl &url) const
{
	for (UrlDownloader *downloader : m_downloaders) {
		if (downloader->canDownload(url)) {
			return downloader;
		}
	}
	return nullptr;
}
