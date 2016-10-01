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
		{
			fileFavorites.write(QString(fav.getName() + "|" + QString::number(fav.getNote()) + "|" + fav.getLastViewed().toString(Qt::ISODate) + "\r\n").toUtf8());
		}
		fileFavorites.close();
	}

	// View it later
	QFile fileKfl(m_path + "/viewitlater.txt");
	if (fileKfl.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		fileKfl.write(m_keptForLater.join("\r\n").toUtf8());
		fileKfl.close();
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


QString Profile::getPath() const			{ return m_path;			}
QSettings *Profile::getSettings() const		{ return m_settings;		}
QList<Favorite> &Profile::getFavorites()	{ return m_favorites;		}
QStringList &Profile::getKeptForLater()		{ return m_keptForLater;	}
QStringList &Profile::getIgnored()			{ return m_ignored;			}
Commands &Profile::getCommands()			{ return *m_commands;		}
