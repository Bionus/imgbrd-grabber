#include "main-screen.h"
#include <QDir>
#include <QEventLoop>
#include <QSettings>
#include <QSslSocket>
#include <QStandardPaths>
#include "downloader/image-downloader.h"
#include "functions.h"
#include "logger.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "settings.h"
#include "utils/logging.h"
#include "models/qml-image.h"
#include "models/qml-site.h"

#define IMAGES_PER_PAGE 20


MainScreen::MainScreen(Profile *profile, QObject *parent)
	: QObject(parent), m_profile(profile)
{
	connect(&Logger::getInstance(), &Logger::newLog, this, &MainScreen::newLog);
	logSystemInformation(m_profile);

	refreshSites();
	refreshSources();

	connect(m_profile, &Profile::sitesChanged, this, &MainScreen::refreshSites);
}

void MainScreen::refreshSites()
{
	qDeleteAll(m_sites);
	m_sites.clear();

	for (Site *site : m_profile->getSites().values()) {
		m_sites.append(new QmlSite(site, this));
	}

	emit sitesChanged();
}

void MainScreen::refreshSources()
{
	m_sources.clear();
	for (Source *source : m_profile->getSources().values()) {
		m_sources.append(source->getName());
	}
	emit sourcesChanged();
}

void MainScreen::search(const QString &siteUrl, const QString &query, int pageNumber, const QString &postFilter)
{
	m_query = query;
	emit queryChanged();

	Site *site = m_profile->getSites().value(siteUrl);
	Page *page = new Page(m_profile, site, m_profile->getSites().values(), query.split(' '), pageNumber, IMAGES_PER_PAGE, postFilter.split(' '), false, this);

	QEventLoop loop;
	QObject::connect(page, &Page::finishedLoading, &loop, &QEventLoop::quit);
	QObject::connect(page, &Page::failedLoading, &loop, &QEventLoop::quit);
	page->load(false);
	loop.exec();

	QList<QSharedPointer<Image>> results = page->images();

	m_results.clear();
	m_results.reserve(results.count());
	for (const QSharedPointer<Image> &img : results) {
		m_results.append(new QmlImage(img, m_profile, this));
	}

	emit resultsChanged();
}

void MainScreen::newLog(const QString &message)
{
	if (!m_log.isEmpty()) {
		m_log += "<br/>";
	}
	m_log += logToHtml(message);

	emit logChanged();
}

void MainScreen::downloadImage(const QSharedPointer<Image> &image)
{
	const QSettings *settings = m_profile->getSettings();
	const QString filename = settings->value("Save/filename", "%md5%.%ext%").toString();
	const QString path = QDir::toNativeSeparators(settings->value("Save/path", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).toString());

	if (filename.isEmpty() || path.isEmpty()) {
		LOG("Empty save filename or directory", Logger::Error);
		return;
	}

	auto downloader = new ImageDownloader(m_profile, image, filename, path, 1, true, true, this, false);
	connect(downloader, &ImageDownloader::saved, downloader, &ImageDownloader::deleteLater);
	downloader->save();
}

QString MainScreen::addSite(const QString &type, const QString &host, bool https)
{
	const auto sources = m_profile->getSources().values();

	// Find the source
	Source *source = nullptr;
	for (Source *src : sources) {
		if (src->getName() == type) {
			source = src;
			break;
		}
	}
	if (source == nullptr) {
		return "Invalid source";
	}

	// Remove unnecessary prefix
	QString url = host;
	if (url.startsWith("http://")) {
		url = url.mid(7);
	} else if (url.startsWith("https://")) {
		url = url.mid(8);
		https = true;
	}
	if (url.endsWith('/')) {
		url = url.left(url.length() - 1);
	}

	// Add site
	Site *site = new Site(host, source);
	m_profile->addSite(site);

	// Set HTTP setting
	if (https) {
		site->setSetting("ssl", true, false);
	}

	return QString();
}

QString MainScreen::toLocalFile(const QString &url)
{
	if (url.startsWith("file:")) {
		QUrl u(url);
		if (u.isValid()) {
			return u.toLocalFile();
		}
	} else if (url.startsWith("content://com.android.externalstorage.documents")) {
		const QString part = QUrl::fromPercentEncoding(url.midRef(47).toLatin1());
		const int index = part.indexOf(':');
		if (index != -1) {
			return "/storage/emulated/0/" + part.mid(index + 1);
		}
	}
	return url;
}

QString MainScreen::settingsFileName() const
{
	return m_profile->getSettings()->fileName();
}

QString MainScreen::getBlacklist()
{
	return m_profile->getBlacklist().toString();
}

void MainScreen::setBlacklist(const QString &text)
{
	Blacklist blacklist;
	for (const QString &tags : text.split("\n", QString::SkipEmptyParts)) {
		blacklist.add(tags.trimmed().split(' ', QString::SkipEmptyParts));
	}
	m_profile->setBlacklistedTags(blacklist);
}

QString MainScreen::getIgnored()
{
	return m_profile->getIgnored().join('\n');
}

void MainScreen::setIgnored(const QString &ignored)
{
	m_profile->setIgnored(ignored.split('\n', QString::SkipEmptyParts));
}
