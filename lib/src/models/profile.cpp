#include "models/profile.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include "commands/commands.h"
#include "functions.h"
#include "models/site.h"
#include "models/source.h"


Profile::Profile()
	: m_settings(Q_NULLPTR), m_commands(Q_NULLPTR)
{}
Profile::Profile(QSettings *settings, QList<Favorite> favorites, QStringList keptForLater, QString path)
	: m_path(std::move(path)), m_settings(settings), m_favorites(std::move(favorites)), m_keptForLater(std::move(keptForLater)), m_commands(Q_NULLPTR)
{}
Profile::Profile(QString path)
	: m_path(std::move(path))
{
	m_settings = new QSettings(m_path + "/settings.ini", QSettings::IniFormat);

	// Load sources
	QStringList dirs = QDir(m_path + "/sites/").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString &dir : dirs)
	{
		Source *source = new Source(this, m_path + "/sites/" + dir);
		if (source->getApis().isEmpty())
		{
			source->deleteLater();
			continue;
		}

		m_sources.insert(source->getName(), source);

		for (Site *site : source->getSites())
			m_sites.insert(site->url(), site);
	}

	// Load favorites
	QSet<QString> unique;
	QFile fileFavoritesJson(m_path + "/favorites.json");
	if (fileFavoritesJson.open(QFile::ReadOnly | QFile::Text))
	{
		const QByteArray data = fileFavoritesJson.readAll();
		QJsonDocument loadDoc = QJsonDocument::fromJson(data);
		QJsonObject object = loadDoc.object();

		QJsonArray favorites = object["favorites"].toArray();
		for (auto favoriteJson : favorites)
		{
			Favorite fav = Favorite::fromJson(m_path, favoriteJson.toObject(), m_sites);
			if (!unique.contains(fav.getName()))
			{
				unique.insert(fav.getName());
				m_favorites.append(fav);
			}
		}
	}
	else
	{
		QFile fileFavorites(m_path + "/favorites.txt");
		if (fileFavorites.open(QFile::ReadOnly | QFile::Text))
		{
			QString favs = fileFavorites.readAll();
			fileFavorites.close();

			QStringList words = favs.split("\n", QString::SkipEmptyParts);
			m_favorites.reserve(words.count());
			for (const QString &word : words)
			{
				Favorite fav = Favorite::fromString(m_path, word);
				if (!unique.contains(fav.getName()))
				{
					unique.insert(fav.getName());
					m_favorites.append(fav);
				}
			}
		}
	}

	// Load view it later
	QFile fileKfl(m_path + "/viewitlater.txt");
	if (fileKfl.open(QFile::ReadOnly | QFile::Text))
	{
		QString vil = fileKfl.readAll();
		fileKfl.close();

		m_keptForLater = vil.split("\n", QString::SkipEmptyParts);
	}

	// Load ignored
	QFile fileIgnored(m_path + "/ignore.txt");
	if (fileIgnored.open(QFile::ReadOnly | QFile::Text))
	{
		QString ign = fileIgnored.readAll();
		fileIgnored.close();

		m_ignored = ign.split("\n", QString::SkipEmptyParts);
	}

	// Load MD5s
	QFile fileMD5(m_path + "/md5s.txt");
	if (fileMD5.open(QFile::ReadOnly | QFile::Text))
	{
		QString line;
		while (!(line = fileMD5.readLine()).isEmpty())
			m_md5s.insert(line.left(32), line.mid(32).trimmed());

		fileMD5.close();
	}

	// Load auto-complete
	QFile fileAutoComplete(m_path + "/words.txt");
	if (fileAutoComplete.open(QFile::ReadOnly | QFile::Text))
	{
		QString line;
		while (!(line = fileAutoComplete.readLine()).isEmpty())
			m_autoComplete.append(line.trimmed().split(" ", QString::SkipEmptyParts));

		fileAutoComplete.close();
	}

	// Load custom auto-complete
	QFile fileCustomAutoComplete(m_path + "/wordsc.txt");
	if (fileCustomAutoComplete.open(QFile::ReadOnly | QFile::Text))
	{
		QString line;
		while (!(line = fileCustomAutoComplete.readLine()).isEmpty())
			m_customAutoComplete.append(line.trimmed().split(" ", QString::SkipEmptyParts));

		fileCustomAutoComplete.close();
	}

	m_commands = new Commands(this);

	// Blacklisted tags
	const QStringList &blacklist = m_settings->value("blacklistedtags").toString().split(' ', QString::SkipEmptyParts);
	for (const QString &bl : blacklist)
	{ m_blacklist.add(bl); }
	QFile fileBlacklist(m_path + "/blacklist.txt");
	if (fileBlacklist.open(QFile::ReadOnly | QFile::Text))
	{
		QString line;
		while (!(line = fileBlacklist.readLine()).isEmpty())
			m_blacklist.add(line.trimmed().split(" ", QString::SkipEmptyParts));

		fileBlacklist.close();
	}
}

Profile::~Profile()
{
	sync();

	if (m_settings != nullptr)
		m_settings->deleteLater();

	qDeleteAll(m_sources);
	delete m_commands;
}


void Profile::sync()
{
	if (m_path.isEmpty())
		return;

	syncFavorites();
	syncKeptForLater();
	syncIgnored();

	// MD5s
	QFile fileMD5(m_path + "/md5s.txt");
	if (fileMD5.open(QFile::WriteOnly | QFile::Truncate))
	{
		QStringList md5s = m_md5s.keys();
		QStringList paths = m_md5s.values();
		for (int i = 0; i < md5s.size(); i++)
			fileMD5.write(QString(md5s[i] + paths[i] + "\n").toUtf8());

		fileMD5.close();
	}

	// Custom auto-complete
	QFile fileCustomAutoComplete(m_path + "/wordsc.txt");
	if (fileCustomAutoComplete.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		fileCustomAutoComplete.write(m_customAutoComplete.join("\r\n").toUtf8());
		fileCustomAutoComplete.close();
	}

	// Update commands settings
	auto *oldCommands = m_commands;
	auto *newCommands = new Commands(this);
	m_commands = newCommands;
	delete oldCommands;

	// Blacklisted tags
	QFile fileBlacklist(m_path + "/blacklist.txt");
	if (fileBlacklist.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		fileBlacklist.write(m_blacklist.toString().toUtf8());
		fileBlacklist.close();
	}
	m_settings->remove("blacklistedtags");

	// Sync settings
	if (m_settings != nullptr)
		m_settings->sync();
}
void Profile::syncFavorites() const
{
	QFile fileFavorites(m_path + "/favorites.json");
	if (fileFavorites.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		// Generate JSON array
		QJsonArray favoritesJson;
		for (const Favorite &fav : qAsConst(m_favorites))
		{
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
		fileFavorites.write(saveDoc.toJson());
		fileFavorites.close();
	}
}
void Profile::syncKeptForLater() const
{
	QFile fileKfl(m_path + "/viewitlater.txt");
	if (fileKfl.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		fileKfl.write(m_keptForLater.join("\r\n").toUtf8());
		fileKfl.close();
	}
}
void Profile::syncIgnored() const
{
	QFile fileIgnored(m_path + "/ignore.txt");
	if (fileIgnored.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		fileIgnored.write(m_ignored.join("\r\n").toUtf8());
		fileIgnored.close();
	}
}

QString Profile::tempPath() const
{
	const QString tmp = QDir::tempPath();
	const QString subDir = "Grabber";
	QDir(tmp).mkpath(subDir);
	return tmp + QDir::separator() + subDir;
}

void Profile::addFavorite(const Favorite &fav)
{
	m_favorites.removeAll(fav);
	m_favorites.append(fav);

	syncFavorites();
	emit favoritesChanged();
}
void Profile::removeFavorite(const Favorite &fav)
{
	m_favorites.removeAll(fav);

	if (QFile::exists(m_path + "/thumbs/" + fav.getName(true) + ".png"))
		QFile::remove(m_path + "/thumbs/" + fav.getName(true) + ".png");

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

QPair<QString, QString> Profile::md5Action(const QString &md5)
{
	QString action = m_settings->value("Save/md5Duplicates", "save").toString();
	const bool keepDeleted = m_settings->value("Save/keepDeletedMd5", false).toBool();

	const bool contains = !md5.isEmpty() && m_md5s.contains(md5);
	QString path = contains ? m_md5s[md5] : QString();
	const bool exists = contains && QFile::exists(path);

	if (contains && !exists)
	{
		if (!keepDeleted)
		{
			removeMd5(md5);
			path = QString();
		}
		else
		{ action = "ignore"; }
	}

	return QPair<QString, QString>(action, path);
}

/**
 * Check if a file with this md5 already exists;
 * @param	md5		The md5 that needs to be checked.
 * @return			A QString containing the path to the already existing file, an empty QString if the md5 does not already exists.
 */
QString Profile::md5Exists(const QString &md5)
{
	if (m_md5s.contains(md5))
	{
		if (QFile::exists(m_md5s[md5]))
			return m_md5s[md5];

		if (!m_settings->value("Save/keepDeletedMd5", false).toBool())
			removeMd5(md5);
	}
	return QString();
}

/**
 * Adds a md5 to the _md5 map and adds it to the md5 file.
 * @param	md5		The md5 to add.
 * @param	path	The path to the image with this md5.
 */
void Profile::addMd5(const QString &md5, const QString &path)
{
	if (!md5.isEmpty())
	{ m_md5s.insert(md5, path); }
}

/**
 * Set a md5 to the _md5 map changing the file it is pointing to.
 * @param	md5		The md5 to add.
 * @param	path	The path to the image with this md5.
 */
void Profile::setMd5(const QString &md5, const QString &path)
{
	m_md5s[md5] = path;
}

/**
 * Removes a md5 from the _md5 map and removes it from the md5 file.
 * @param	md5		The md5 to remove.
 */
void Profile::removeMd5(const QString &md5)
{
	m_md5s.remove(md5);
}


void Profile::addAutoComplete(const QString &tag)
{
	m_customAutoComplete.append(tag);
}


void Profile::addSite(Site *site)
{
	m_sites.insert(site->url(), site);
	emit sitesChanged();
}

void Profile::removeSite(Site *site)
{
	m_sites.remove(site->url());
	emit siteDeleted(site);
	emit sitesChanged();
}


void Profile::setBlacklistedTags(const Blacklist &blacklist)
{
	m_blacklist = blacklist;
	emit blacklistChanged();
}

void Profile::addBlacklistedTag(const QString &tag)
{
	m_blacklist.add(tag);
	emit blacklistChanged();
}

void Profile::removeBlacklistedTag(const QString &tag)
{
	m_blacklist.remove(tag);
	emit blacklistChanged();
}


QString Profile::getPath() const				{ return m_path;				}
QSettings *Profile::getSettings() const			{ return m_settings;			}
QList<Favorite> &Profile::getFavorites()		{ return m_favorites;			}
QStringList &Profile::getKeptForLater()			{ return m_keptForLater;		}
QStringList &Profile::getIgnored()				{ return m_ignored;				}
Commands &Profile::getCommands()				{ return *m_commands;			}
QStringList &Profile::getAutoComplete()			{ return m_autoComplete;		}
QStringList &Profile::getCustomAutoComplete()	{ return m_customAutoComplete;	}
Blacklist &Profile::getBlacklist()				{ return m_blacklist;			}
const QMap<QString, Source*> &Profile::getSources() const	{ return m_sources;	}
const QMap<QString, Site*> &Profile::getSites() const		{ return m_sites;	}

QList<Site*> Profile::getFilteredSites(const QStringList &urls) const
{
	QList<Site*> ret;
	for (const QString &url : urls)
		if (m_sites.contains(url))
			ret.append(m_sites.value(url));
	return ret;
}
