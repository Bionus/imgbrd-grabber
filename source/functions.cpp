#include <QApplication>
#include <QtGui>
#include "functions.h"



/**
 * Popup a message notifying the user that something went wrong.
 * @param parent	The parent widget
 * @param error		The error message
 */
void error(QWidget *parent, QString error)
{ QMessageBox::critical(parent, "Error", error); }

/**
 * Sort a list non case-sensitively.
 * @param sList		The list that will be ordered
 */
void sortNonCaseSensitive(QStringList &sList)
{
	QMap<QString, QString> strMap;
	foreach (QString str, sList)
	{ strMap.insert( str.toLower(), str); }
	sList = strMap.values();
}

/**
 * Convert a danbooru-like date (Sat May 14 17:38:04 -0400 2011) to a valid QDateTime.
 * @param	str		The date string
 * @return	The converted date as QDateTime
 */
QDateTime qDateTimeFromString(QString str, int timezonedecay)
{
	QDateTime date;
	QList<QString> months;
	months << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" << "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";
	QTime time(str.mid(11,2).toInt(), str.mid(14,2).toInt(), str.mid(17,2).toInt());
	date.setDate(QDate(str.mid(26,4).toInt(), months.indexOf(str.mid(4,3))+1, str.mid(8,2).toInt()+(str.mid(11,2).toInt() >= 18)));
	date.setTime(time.addSecs(3600*timezonedecay)); // We convert the date to user's time
	return date;
}

/**
 * Load favorites from local file.
 * @return	A QMap<QString,QString> with tags as keys, then the remaining details as value (value1|value2|value3...)
 */
QMap<QString,QString> loadFavorites()
{
	QMap<QString,QString> favorites;
	QFile file(savePath("favorites.txt"));
	if (file.open(QIODevice::ReadOnly))
	{
		QString favs = file.readAll();
		QStringList wrds = favs.replace("\r\n", "\n").replace("\r", "\n").split("\n");
		for (int i = 0; i < wrds.count(); i++)
		{
			if (!wrds.at(i).isEmpty())
			{
				QStringList xp = wrds.at(i).split("|");
				QString tag = xp.takeFirst();
				favorites.insert(tag, (xp.isEmpty() ? "" : xp.join("|")));
			}
		}
		file.close();
	}
	return favorites;
}

QString validateFilename(QString text)
{
	if (text.isEmpty())
	{ return QObject::tr("<span style=\"color:red\">Les noms de fichiers ne doivent pas être vides !</span>"); }
	if (!text.endsWith(".%ext%"))
	{ return QObject::tr("<span style=\"color:orange\">Votre nom de fichier ne finit pas par une extension, symbolisée par %ext% ! Vous risquez de ne pas pouvoir ouvrir vos fichiers.</span>"); }
	if (!text.contains("%md5%"))
	{ return QObject::tr("<span style=\"color:orange\">Votre nom de fichier n'est pas unique à chaque image et une image risque d'en écraser une précédente lors de la sauvegarde ! Vous devriez utiliser le symbole %md5%, unique à chaque image, pour éviter ce désagrément.</span>"); }
	return QObject::tr("<span style=\"color:green\">Format valide !</span>");
}

QString savePath(QString file)
{ return QDir::toNativeSeparators(QDir::homePath()+"/Grabber/"+file); }

void showInGraphicalShell(const QString &pathIn)
{
	// Mac & Windows support folder or file.
	#if defined(Q_OS_WIN)
		QString param;
		if (!QFileInfo(pathIn).isDir())
		{ param = QLatin1String("/select,"); }
		param += QDir::toNativeSeparators(pathIn);
		QProcess::startDetached("explorer.exe "+param);
	#elif defined(Q_OS_MAC)
		Q_UNUSED(parent)
		QStringList scriptArgs;
		scriptArgs << QLatin1String("-e") << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(pathIn);
		QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
		scriptArgs.clear();
		scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
		QProcess::execute("/usr/bin/osascript", scriptArgs);
	#else
		// we cannot select a file here, because no file browser really supports it...
		const QFileInfo fileInfo(pathIn);
		const QString folder = fileInfo.absoluteFilePath();
		const QString app = Utils::UnixUtils::fileBrowser(Core::ICore::instance()->settings());
		QProcess browserProc;
		const QString browserArgs = Utils::UnixUtils::substituteFileBrowserParameters(app, folder);
		bool success = browserProc.startDetached(browserArgs);
		const QString error = QString::fromLocal8Bit(browserProc.readAllStandardError());
		success = success && error.isEmpty();
		//if (!success)
		//{ showGraphicalShellError(parent, app, error); }
	#endif
}

