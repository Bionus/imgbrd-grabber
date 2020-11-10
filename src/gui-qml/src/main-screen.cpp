#include "main-screen.h"
#include <QDir>
#include <QEventLoop>
#include <QSettings>
#include <QSslSocket>
#include <QStandardPaths>
#include "downloader/image-downloader.h"
#include "functions.h"
#include "logger.h"
#include "models/favorite.h"
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
	refreshFavorites();

	connect(m_profile, &Profile::sitesChanged, this, &MainScreen::refreshSites);
	connect(m_profile, &Profile::favoritesChanged, this, &MainScreen::refreshFavorites);
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

void MainScreen::refreshFavorites()
{
	m_favorites.clear();
	for (const Favorite &fav : m_profile->getFavorites()) {
		m_favorites.append(fav.getName(false));
	}
	emit favoritesChanged();
}

void MainScreen::search(const QString &siteUrl, const QString &query, int pageNumber, const QString &postFilter)
{
	m_query = query;
	emit queryChanged();

	Site *site = m_profile->getSites().value(siteUrl);
	Page *page = new Page(m_profile, site, m_profile->getSites().values(), query.split(' '), pageNumber, IMAGES_PER_PAGE, postFilter.split(' '), false, this);

	connect(page, &Page::finishedLoading, this, &MainScreen::searchFinished);
	connect(page, &Page::failedLoading, this, &MainScreen::searchFinished);
	page->load(false);

}
void MainScreen::searchFinished(Page *page)
{
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

void MainScreen::addFavorite(const QString &query, const QString &siteUrl)
{
	Favorite fav(query);
	fav.setSites({ m_profile->getSites().value(siteUrl) });
	m_profile->addFavorite(fav);
}
void MainScreen::removeFavorite(const QString &query)
{
	m_profile->removeFavorite(Favorite(query));
}

void MainScreen::loadSuggestions(const QString &prefix, int limit)
{
	m_autoComplete.clear();

	// Ignore empty searches or completions right after a space
	if (prefix.isEmpty()) {
		emit autoCompleteChanged();
		return;
	}

	// Get the first element for which the comparison "elt < prefix" is wrong
	const auto &cmp = m_profile->getAutoComplete();
	auto it = std::lower_bound(cmp.constBegin(), cmp.constEnd(), prefix);

	// Get max $limit results starting with the prefix
	while (it != cmp.constEnd() && it->startsWith(prefix) && m_autoComplete.count() < limit) {
		m_autoComplete.append(*it);
		++it;
	}

	emit autoCompleteChanged();
}

bool MainScreen::exportSettings(const QString &dest)
{
	return QFile::copy(m_profile->getSettings()->fileName(), dest);
}

bool MainScreen::importSettings(const QString &source)
{
	QSettings sourceSettings(source, QSettings::IniFormat);
	if (sourceSettings.status() != QSettings::NoError) {
		return false;
	}

	QSettings *settings = m_profile->getSettings();
	settings->clear();

	for (const QString &key : sourceSettings.allKeys()) {
		settings->setValue(key, sourceSettings.value(key));
	}

	settings->sync();
	emit settingsChanged();

	return true;
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
