#include "profile.h"
#include <QFile>


Profile::Profile(QString path)
	: m_path(path)
{
	m_settings = new QSettings(m_path + "/settings.ini", QSettings::IniFormat);

	// Load favorites
	QFile fileFavorites(m_path + "/favorites.txt");
	if (fileFavorites.open(QIODevice::ReadOnly))
	{
		QString favs = fileFavorites.readAll();
		QStringList wrds = favs.replace("\r\n", "\n").replace("\r", "\n").split("\n");
		for (QString wrd : wrds)
		{
			if (!wrd.isEmpty())
			{
				QStringList xp = wrd.split("|");
				QString tag = xp.takeFirst();
				QString thumbPath = m_path + "/thumbs/" + (QString(tag).remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|')) + ".png";

				Favorite fav(tag,
							 xp.isEmpty() ? 50 : xp.takeFirst().toInt(),
							 xp.isEmpty() ? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)) : QDateTime::fromString(xp.takeFirst(), Qt::ISODate),
							 QFile::exists(thumbPath) ? thumbPath : ":/images/noimage.png");
				m_favorites.append(fav);
			}
		}
		fileFavorites.close();
	}

	// Load view it later
	QFile fileKfl(m_path + "/viewitlater.txt");
	if (fileKfl.open(QIODevice::ReadOnly))
	{
		QString vil = fileKfl.readAll();
		m_keptForLater = vil.replace("\r\n", "\n").replace("\r", "\n").split("\n");
		fileKfl.close();
	}
}

Profile::~Profile()
{
	sync();

	m_settings->deleteLater();
}


void Profile::sync()
{
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


QString Profile::getPath() const						{ return m_path;			}
QSettings *Profile::getSettings() const					{ return m_settings;		}
const QList<Favorite> &Profile::getFavorites() const	{ return m_favorites;		}
const QList<QString> &Profile::getKeptForLater() const	{ return m_keptForLater;	}
