#include "profile.h"
#include <QFile>
#include <QDir>
#include "site.h"
#include "commands/commands.h"


Profile::Profile()
	: m_settings(nullptr), m_commands(nullptr)
{}
Profile::Profile(QSettings *settings, QList<Favorite> favorites, QStringList keptForLater, QString path)
	: m_path(path), m_settings(settings), m_favorites(favorites), m_keptForLater(keptForLater), m_commands(nullptr)
{}
Profile::Profile(QString path)
	: m_path(path)
{
	m_settings = new QSettings(m_path + "/settings.ini", QSettings::IniFormat);

	// Load favorites
	QFile fileFavorites(m_path + "/favorites.txt");
	if (fileFavorites.open(QFile::ReadOnly | QFile::Text))
	{
		QString favs = fileFavorites.readAll();
		fileFavorites.close();

		QSet<QString> unique;
		QStringList wrds = favs.split("\n", QString::SkipEmptyParts);
		for (const QString &wrd : wrds)
		{
			Favorite fav = Favorite::fromString(m_path, wrd);
			if (!unique.contains(fav.getName()))
			{
				unique.insert(fav.getName());
				m_favorites.append(fav);
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
	m_blacklistedTags = m_settings->value("blacklistedtags").toString().split(' ', QString::SkipEmptyParts);
}

Profile::~Profile()
{
	sync();

	if (m_settings != nullptr)
		m_settings->deleteLater();

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
	m_settings->setValue("blacklistedtags", m_blacklistedTags.join(' '));

	// Sync settings
	if (m_settings != nullptr)
		m_settings->sync();
}
void Profile::syncFavorites()
{
	QFile fileFavorites(m_path + "/favorites.txt");
	if (fileFavorites.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		for (const Favorite &fav : m_favorites)
			fileFavorites.write(QString(fav.getName() + "|" + QString::number(fav.getNote()) + "|" + fav.getLastViewed().toString(Qt::ISODate) + "\r\n").toUtf8());

		fileFavorites.close();
	}
}
void Profile::syncKeptForLater()
{
	QFile fileKfl(m_path + "/viewitlater.txt");
	if (fileKfl.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		fileKfl.write(m_keptForLater.join("\r\n").toUtf8());
		fileKfl.close();
	}
}
void Profile::syncIgnored()
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
	QString tmp = QDir::tempPath();
	QString subDir = "Grabber";
	QDir(tmp).mkpath(subDir);
	return tmp + QDir::separator() + subDir;
}

void Profile::addFavorite(Favorite fav)
{
	m_favorites.removeAll(fav);
	m_favorites.append(fav);

	syncFavorites();
	emit favoritesChanged();
}
void Profile::removeFavorite(Favorite fav)
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

void Profile::addKeptForLater(QString tag)
{
	m_keptForLater.removeAll(tag);
	m_keptForLater.append(tag);

	syncKeptForLater();
	emit keptForLaterChanged();
}
void Profile::removeKeptForLater(QString tag)
{
	m_keptForLater.removeAll(tag);

	syncKeptForLater();
	emit keptForLaterChanged();
}

void Profile::addIgnored(QString tag)
{
	m_ignored.removeAll(tag);
	m_ignored.append(tag);

	syncIgnored();
	emit ignoredChanged();
}
void Profile::removeIgnored(QString tag)
{
	m_ignored.removeAll(tag);

	syncIgnored();
	emit ignoredChanged();
}

QPair<QString, QString> Profile::md5Action(QString md5)
{
	QString action = m_settings->value("Save/md5Duplicates", "save").toString();
	bool keepDeleted = m_settings->value("Save/keepDeletedMd5", false).toBool();

	bool contains = m_md5s.contains(md5);
	QString path = contains ? m_md5s[md5] : QString();
	bool exists = contains && QFile::exists(path);

	if (contains && !exists && !keepDeleted)
	{
		removeMd5(md5);
		path = QString();
	}

	else if (contains && !exists && keepDeleted)
		action = "ignore";

	return QPair<QString, QString>(action, path);
}

/**
 * Check if a file with this md5 already exists;
 * @param	md5		The md5 that needs to be checked.
 * @return			A QString containing the path to the already existing file, an empty QString if the md5 does not already exists.
 */
QString Profile::md5Exists(QString md5)
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
void Profile::addMd5(QString md5, QString path)
{
	m_md5s.insert(md5, path);
}

/**
 * Set a md5 to the _md5 map changing the file it is pointing to.
 * @param	md5		The md5 to add.
 * @param	path	The path to the image with this md5.
 */
void Profile::setMd5(QString md5, QString path)
{
	m_md5s[md5] = path;
}

/**
 * Removes a md5 from the _md5 map and removes it from the md5 file.
 * @param	md5		The md5 to remove.
 */
void Profile::removeMd5(QString md5)
{
	m_md5s.remove(md5);
}


void Profile::addAutoComplete(QString tag)
{
	m_customAutoComplete.append(tag);
}


void Profile::addSite(Site *site)
{
	Q_UNUSED(site);
	emit sitesChanged();
}


void Profile::setBlacklistedTags(QStringList tags)
{
	m_blacklistedTags = tags;
	emit blacklistChanged();
}

void Profile::addBlacklistedTag(QString tag)
{
	m_blacklistedTags.append(tag);
	emit blacklistChanged();
}

void Profile::removeBlacklistedTag(QString tag)
{
	m_blacklistedTags.removeAll(tag);
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
QStringList &Profile::getBlacklist()			{ return m_blacklistedTags;		}
