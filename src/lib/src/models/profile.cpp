#include "models/profile.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <QSettings>
#include <utility>
#include "commands/commands.h"
#include "downloader/download-query-manager.h"
#include "exiftool.h"
#include "functions.h"
#include "logger.h"
#include "models/favorite.h"
#include "models/md5-database/md5-database-sqlite.h"
#include "models/md5-database/md5-database-text.h"
#include "models/site.h"
#include "models/source.h"
#include "models/source-registry.h"
#include "models/url-downloader/url-downloader-manager.h"
#include "monitoring/monitor-manager.h"
#include "utils/file-utils.h"
#include "utils/read-write-path.h"


Profile::Profile()
	: m_settings(nullptr), m_commands(nullptr), m_exiftool(nullptr), m_md5s(nullptr), m_monitorManager(nullptr), m_downloadQueryManager(nullptr), m_urlDownloaderManager(nullptr)
{}
Profile::Profile(QSettings *settings, QList<Favorite> favorites, QStringList keptForLater, QString path)
	: m_path(std::move(path)), m_settings(settings), m_favorites(std::move(favorites)), m_keptForLater(std::move(keptForLater)), m_commands(nullptr), m_exiftool(nullptr), m_md5s(nullptr), m_monitorManager(nullptr), m_downloadQueryManager(nullptr), m_urlDownloaderManager(nullptr)
{}
Profile::Profile(QString path)
	: m_path(std::move(path)), m_urlDownloaderManager(nullptr)
{
	m_settings = new QSettings(m_path + "/settings.ini", QSettings::IniFormat);

	// Rename deprecated settings keys
	renameSettingsGroup(m_settings, "Zoom", "Viewer");

	// Load sources
	const QString defaultPath = savePath("sites/", true, false);
	const QString customPath = m_path + "/sites/";
	QStringList sites = QDir(customPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	if (!defaultPath.startsWith(m_path)) {
		sites += QDir(defaultPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		sites.removeDuplicates();
	}
	for (const QString &dir : sites) {
		const QString readDir = defaultPath + dir;
		const QString writeDir = customPath + dir;
		auto *source = new Source(ReadWritePath(readDir, writeDir));
		if (!source->isValid()) {
			source->deleteLater();
			continue;
		}

		addSource(source);
	}

	// Load favorites
	QSet<QString> unique;
	QFile fileFavoritesJson(m_path + "/favorites.json");
	if (fileFavoritesJson.open(QFile::ReadOnly | QFile::Text)) {
		const QByteArray data = fileFavoritesJson.readAll();
		QJsonDocument loadDoc = QJsonDocument::fromJson(data);
		QJsonObject object = loadDoc.object();

		QJsonArray favorites = object["favorites"].toArray();
		for (auto favoriteJson : favorites) {
			Favorite fav = Favorite::fromJson(m_path, favoriteJson.toObject(), this);
			if (!unique.contains(fav.getName())) {
				unique.insert(fav.getName());
				m_favorites.append(fav);
			}
		}
	} else {
		QFile fileFavorites(m_path + "/favorites.txt");
		if (fileFavorites.open(QFile::ReadOnly | QFile::Text)) {
			QString favorites = fileFavorites.readAll();
			fileFavorites.close();

			QStringList words = favorites.split("\n", Qt::SkipEmptyParts);
			m_favorites.reserve(words.count());
			for (const QString &word : words) {
				Favorite fav = Favorite::fromString(m_path, word);
				if (!unique.contains(fav.getName())) {
					unique.insert(fav.getName());
					m_favorites.append(fav);
				}
			}
		}
	}

	// Load view it later
	QFile fileKfl(m_path + "/viewitlater.txt");
	if (fileKfl.open(QFile::ReadOnly | QFile::Text)) {
		QString vil = fileKfl.readAll();
		fileKfl.close();

		m_keptForLater = vil.split("\n", Qt::SkipEmptyParts);
	}

	// Load ignored
	QFile fileIgnored(m_path + "/ignore.txt");
	if (fileIgnored.open(QFile::ReadOnly | QFile::Text)) {
		QString ign = fileIgnored.readAll();
		fileIgnored.close();

		m_ignored = ign.split("\n", Qt::SkipEmptyParts);
	}

	// Load removed
	m_removedTags.add(splitStringMulti({ ' ', '\n' }, m_settings->value("ignoredtags").toString(), true));

	// Make a backup of MD5s in case the multi-location change broke everything
	if (QFile::exists(m_path + "/md5s.txt") && !QFile::exists(m_path + "/md5s.txt.bak")) {
		QFile::copy(m_path + "/md5s.txt", m_path + "/md5s.txt.bak");
	}

	// Load MD5s
	m_md5s = QFile::exists(m_path + "/md5s.sqlite") || !QFile::exists(m_path + "/md5s.txt")
		? (Md5Database*) new Md5DatabaseSqlite(m_path + "/md5s.sqlite", m_settings)
		: (Md5Database*) new Md5DatabaseText(m_path + "/md5s.txt", m_settings);

	// Load auto-complete
	QFile fileAutoComplete(savePath("words.txt", true, false));
	if (fileAutoComplete.open(QFile::ReadOnly | QFile::Text)) {
		QString line;
		while (!(line = fileAutoComplete.readLine()).isEmpty()) {
			m_autoComplete.append(line.trimmed().split(" ", Qt::SkipEmptyParts));
		}

		fileAutoComplete.close();
	}

	// Load custom auto-complete
	QFile fileCustomAutoComplete(m_path + "/wordsc.txt");
	if (fileCustomAutoComplete.open(QFile::ReadOnly | QFile::Text)) {
		QString line;
		while (!(line = fileCustomAutoComplete.readLine()).isEmpty()) {
			m_customAutoComplete.append(line.trimmed().split(" ", Qt::SkipEmptyParts));
		}

		fileCustomAutoComplete.close();
	}

	m_commands = new Commands(this);
	m_exiftool = new Exiftool(this);

	// Blacklisted tags
	const QStringList &blacklist = m_settings->value("blacklistedtags").toString().split(' ', Qt::SkipEmptyParts);
	for (const QString &bl : blacklist) {
		m_blacklist.add(bl);
	}
	QFile fileBlacklist(m_path + "/blacklist.txt");
	if (fileBlacklist.open(QFile::ReadOnly | QFile::Text)) {
		QString line;
		while (!(line = fileBlacklist.readLine()).isEmpty()) {
			line = line.trimmed();
			if (!line.startsWith('#')) {
				m_blacklist.add(line.split(" ", Qt::SkipEmptyParts));
			}
		}

		fileBlacklist.close();
	}

	// Monitors
	m_monitorManager = new MonitorManager(m_path + "/monitors.json", this);

	// Downloads
	m_downloadQueryManager = new DownloadQueryManager(m_path + "/restore.igl", this);

	// URL downloaders
	m_urlDownloaderManager = new UrlDownloaderManager(ReadWritePath(defaultPath, customPath), this);

	// Complete auto-complete
	static QStringList specialCompletes = { "grabber:alreadyExists", "grabber:inMd5List", "grabber:downloaded", "grabber:favorited" };
	m_autoComplete.reserve(m_autoComplete.count() + m_customAutoComplete.count() + m_favorites.count() + specialCompletes.count());
	m_autoComplete.append(m_customAutoComplete);
	for (const Favorite &fav : qAsConst(m_favorites)) {
		m_autoComplete.append(fav.getName());
	}
	m_autoComplete.append(specialCompletes);
	m_autoComplete.removeDuplicates();
	m_autoComplete.sort();

	// Load source registries
	const QStringList sourceRegistries = m_settings->value("sourceRegistries").toStringList();
	for (const QString &url : sourceRegistries) {
		auto *sourceRegistry = new SourceRegistry(url);
		auto receiver = new QObject(this);
		connect(sourceRegistry, &SourceRegistry::loaded, receiver, [=](bool ok) {
			receiver->deleteLater();
			if (ok) {
				m_sourceRegistries.append(sourceRegistry);
				emit sourceRegistriesChanged();
			} else {
				log(QStringLiteral("Error loading source registry `%1`").arg(url), Logger::Warning);
				sourceRegistry->deleteLater();
			}
		});
		sourceRegistry->load();
	}
}

Profile::~Profile()
{
	sync();

	if (m_settings != nullptr) {
		m_settings->deleteLater();
	}

	qDeleteAll(m_sources);
	delete m_commands;
	delete m_md5s;
	delete m_monitorManager;
	delete m_downloadQueryManager;
	delete m_urlDownloaderManager;
	qDeleteAll(m_sourceRegistries);

	if (m_exiftool != nullptr) {
		m_exiftool->stop();
	}
}


void Profile::sync()
{
	if (m_path.isEmpty()) {
		return;
	}

	syncFavorites();
	syncKeptForLater();
	syncIgnored();
	syncBlacklist();

	// MD5s
	m_md5s->sync();

	// Custom auto-complete
	QFile fileCustomAutoComplete(m_path + "/wordsc.txt");
	if (fileCustomAutoComplete.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		fileCustomAutoComplete.write(m_customAutoComplete.join("\r\n").toUtf8());
		fileCustomAutoComplete.close();
	}

	// Update commands settings
	auto *oldCommands = m_commands;
	auto *newCommands = new Commands(this);
	m_commands = newCommands;
	delete oldCommands;

	// Sync settings
	if (m_settings != nullptr) {
		m_settings->sync();
	}
}
void Profile::syncFavorites() const
{
	// Generate JSON array
	QJsonArray favoritesJson;
	for (const Favorite &fav : qAsConst(m_favorites)) {
		QJsonObject unique;
		fav.toJson(unique);
		favoritesJson.append(unique);
	}

	// Generate result
	QJsonObject full;
	full["version"] = 1;
	full["favorites"] = favoritesJson;

	// Write result
	QJsonDocument saveDoc(full);
	safeWriteFile(m_path + "/favorites.json", saveDoc.toJson());
}
void Profile::syncKeptForLater() const
{
	safeWriteFile(m_path + "/viewitlater.txt", m_keptForLater.join("\r\n").toUtf8());
}
void Profile::syncIgnored() const
{
	safeWriteFile(m_path + "/ignore.txt", m_ignored.join("\r\n").toUtf8());
}
void Profile::syncBlacklist() const
{
	safeWriteFile(m_path + "/blacklist.txt", m_blacklist.toString().toUtf8());
	m_settings->remove("blacklistedtags");
}

QString Profile::tempPath() const
{
	QString override = m_settings->value("tempPathOverride", "").toString();
	if (!override.isEmpty() && QFile::exists(override)) {
		return override;
	}

	const QString tmp = QDir::tempPath();
	const QString subDir = "Grabber";
	QDir(tmp).mkpath(subDir);
	return tmp + QDir::separator() + subDir;
}

void Profile::purgeTemp(int maxAge) const
{
	const QDir tempDir(tempPath());
	const QFileInfoList tempFiles = tempDir.entryInfoList(QDir::Files);
	const QDateTime max = QDateTime::currentDateTime().addSecs(-maxAge);

	int purged = 0;
	int failed = 0;
	for (const QFileInfo &tempFile : tempFiles) {
		const QDateTime lastModified = tempFile.lastModified();
		if (lastModified < max) {
			if (QFile::remove(tempFile.absoluteFilePath())) {
				purged++;
			} else {
				failed++;
			}
		}
	}

	log(QString("Temp directory purged of %1/%2 files (%3 failed)").arg(purged).arg(tempFiles.count()).arg(failed), Logger::Info);
}

void Profile::addFavorite(const Favorite &fav)
{
	const int already = m_favorites.removeAll(fav);
	m_favorites.append(fav);

	if (already == 0) {
		m_autoComplete.append(fav.getName());
	}

	syncFavorites();
	emit favoritesChanged();
}
void Profile::removeFavorite(const Favorite &fav)
{
	m_favorites.removeAll(fav);

	if (QFile::exists(m_path + "/thumbs/" + fav.getName(true) + ".png")) {
		QFile::remove(m_path + "/thumbs/" + fav.getName(true) + ".png");
	}

	syncFavorites();
	emit favoritesChanged();
}
void Profile::emitFavorite()
{
	syncFavorites();
	emit favoritesChanged();
}

void Profile::addKeptForLater(const QString &tag)
{
	m_keptForLater.removeAll(tag);
	m_keptForLater.append(tag);

	syncKeptForLater();
	emit keptForLaterChanged();
}
void Profile::removeKeptForLater(const QString &tag)
{
	m_keptForLater.removeAll(tag);

	syncKeptForLater();
	emit keptForLaterChanged();
}

void Profile::setIgnored(const QStringList &tags)
{
	m_ignored = tags;

	syncIgnored();
	emit ignoredChanged();
}
void Profile::addIgnored(const QString &tag)
{
	m_ignored.removeAll(tag);
	m_ignored.append(tag);

	syncIgnored();
	emit ignoredChanged();
}
void Profile::removeIgnored(const QString &tag)
{
	m_ignored.removeAll(tag);

	syncIgnored();
	emit ignoredChanged();
}

void Profile::setRemovedTags(const QString &raw)
{
	m_removedTags.clear();
	m_removedTags.add(splitStringMulti({ ' ', '\n' }, raw, true));

	m_settings->setValue("ignoredtags", raw);
}

QPair<QString, QString> Profile::md5Action(const QString &md5, const QString &target)
{
	return m_md5s->action(md5, target);
}

/**
 * Check if a file with this md5 already exists;
 * @param	md5		The md5 that needs to be checked.
 * @return			A QString containing the path to the already existing file, an empty QString if the md5 does not already exists.
 */
QStringList Profile::md5Exists(const QString &md5)
{
	return m_md5s->exists(md5);
}

/**
 * Adds a md5 to the _md5 map and adds it to the md5 file.
 * @param	md5		The md5 to add.
 * @param	path	The path to the image with this md5.
 */
void Profile::addMd5(const QString &md5, const QString &path)
{
	m_md5s->add(md5, path);
}

/**
 * Removes a md5 from the _md5 map and removes it from the md5 file.
 * @param	md5		The md5 to remove.
 */
void Profile::removeMd5(const QString &md5, const QString &path)
{
	m_md5s->remove(md5, path);
}


void Profile::addAutoComplete(const QString &tag)
{
	m_customAutoComplete.append(tag);
}


void Profile::addSource(Source *source)
{
	m_sources.insert(source->getName(), source);
	m_additionalTokens.append(source->getAdditionalTokens());

	for (const QString &site : source->getSites()) {
		m_sites.insert(site, new Site(site, source, this));
	}
}

void Profile::addSite(Site *site)
{
	m_sites.insert(site->url(), site);
	m_sources[site->type()]->addSite(site->url());

	emit sitesChanged();
}

void Profile::removeSite(Site *site)
{
	m_sites.remove(site->url());
	m_sources[site->type()]->removeSite(site->url());

	emit siteDeleted(site);
	emit sitesChanged();
}


void Profile::setBlacklistedTags(const Blacklist &blacklist)
{
	m_blacklist = blacklist;

	syncBlacklist();
	emit blacklistChanged();
}

void Profile::addBlacklistedTag(const QString &tag)
{
	m_blacklist.add(QString(tag).replace(":", "::"));

	syncBlacklist();
	emit blacklistChanged();
}

void Profile::removeBlacklistedTag(const QString &tag)
{
	m_blacklist.remove(tag);

	syncBlacklist();
	emit blacklistChanged();
}


const QList<SourceRegistry*> &Profile::getSourceRegistries() const
{
	return m_sourceRegistries;
}

void Profile::addSourceRegistry(SourceRegistry *sourceRegistry)
{
	m_sourceRegistries.append(sourceRegistry);
	syncSourceRegistries();
	emit sourceRegistriesChanged();
}

void Profile::removeSourceRegistry(SourceRegistry *sourceRegistry)
{
	if (m_sourceRegistries.removeAll(sourceRegistry) > 0) {
		syncSourceRegistries();
		emit sourceRegistriesChanged();
	}
}

void Profile::syncSourceRegistries()
{
	QStringList sourceRegistries;
	sourceRegistries.reserve(m_sourceRegistries.size());
	for (SourceRegistry *sourceRegistry : m_sourceRegistries) {
		sourceRegistries.append(sourceRegistry->jsonUrl());
	}
	m_settings->setValue("sourceRegistries", sourceRegistries);
}


QString Profile::getPath() const { return m_path; }
QSettings *Profile::getSettings() const { return m_settings; }
QList<Favorite> &Profile::getFavorites() { return m_favorites; }
QStringList &Profile::getKeptForLater() { return m_keptForLater; }
QStringList &Profile::getIgnored() { return m_ignored; }
TagFilterList &Profile::getRemovedTags() { return m_removedTags; }
Commands &Profile::getCommands() { return *m_commands; }
Exiftool &Profile::getExiftool() { return *m_exiftool; }
QStringList &Profile::getAutoComplete() { return m_autoComplete; }
Blacklist &Profile::getBlacklist() { return m_blacklist; }
const QMap<QString, Source*> &Profile::getSources() const { return m_sources; }
const QMap<QString, Site*> &Profile::getSites() const { return m_sites; }
const QStringList &Profile::getAdditionalTokens() const { return m_additionalTokens; }
MonitorManager *Profile::monitorManager() const { return m_monitorManager; }
DownloadQueryManager *Profile::downloadQueryManager() const { return m_downloadQueryManager; }
UrlDownloaderManager *Profile::urlDownloaderManager() const { return m_urlDownloaderManager; }
Md5Database *Profile::md5Database() const { return m_md5s; }

QList<Site*> Profile::getFilteredSites(const QStringList &urls) const
{
	QList<Site*> ret;
	for (const QString &url : urls) {
		if (m_sites.contains(url)) {
			ret.append(m_sites.value(url));
		} else {
			log(QStringLiteral("Unknown site: %1").arg(url), Logger::Error);
		}
	}
	return ret;
}
