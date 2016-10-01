#include <QFile>
#include "profile.h"
#include "commands.h"


Profile::Profile()
	: m_settings(nullptr), m_commands(nullptr)
{}
Profile::Profile(QSettings *settings, QList<Favorite> favorites, QStringList keptForLater, QString path)
	: m_path(path), m_settings(settings), m_favorites(favorites), m_keptForLater(keptForLater)
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

		QStringList wrds = favs.split("\n", QString::SkipEmptyParts);
		for (QString wrd : wrds)
		{
			m_favorites.append(Favorite::fromString(m_path, wrd));
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

	// Load MD5s
	QFile fileMD5(m_path + "/md5s.txt");
	if (fileMD5.open(QFile::ReadOnly | QFile::Text))
	{
		QString line;
		while (!(line = fileMD5.readLine()).isEmpty())
			m_md5s.insert(line.left(32), line.mid(32));

		fileMD5.close();
	}

	m_commands = new Commands(this);
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
	if (m_settings != nullptr)
		m_settings->sync();

	if (m_path.isEmpty())
		return;

	// Favorites
	QFile fileFavorites(m_path + "/favorites.txt");
	if (fileFavorites.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		for (Favorite fav : m_favorites)
			fileFavorites.write(QString(fav.getName() + "|" + QString::number(fav.getNote()) + "|" + fav.getLastViewed().toString(Qt::ISODate) + "\r\n").toUtf8());

		fileFavorites.close();
	}

	// View it later
	QFile fileKfl(m_path + "/viewitlater.txt");
	if (fileKfl.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		fileKfl.write(m_keptForLater.join("\r\n").toUtf8());
		fileKfl.close();
	}

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
}

void Profile::addFavorite(Favorite fav)
{
	m_favorites.removeAll(fav);
	m_favorites.append(fav);
}
void Profile::removeFavorite(Favorite fav)
{
	m_favorites.removeAll(fav);

	if (QFile::exists(m_path + "/thumbs/" + fav.getName(true) + ".png"))
		QFile::remove(m_path + "/thumbs/" + fav.getName(true) + ".png");
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


QString Profile::getPath() const			{ return m_path;			}
QSettings *Profile::getSettings() const		{ return m_settings;		}
QList<Favorite> &Profile::getFavorites()	{ return m_favorites;		}
QStringList &Profile::getKeptForLater()		{ return m_keptForLater;	}
QStringList &Profile::getIgnored()			{ return m_ignored;			}
Commands &Profile::getCommands()			{ return *m_commands;		}
