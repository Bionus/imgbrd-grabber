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

/**
 * Check filename format's validity.
 * @param	text	The format to be validated.
 * @return			A QString containing the message (error or not).
 * @todo			Return a constant instead of a QString.
 */
QString validateFilename(QString text)
{
	// Field must be filled
	if (text.isEmpty())
	{ return QObject::tr("<span style=\"color:red\">Les noms de fichiers ne doivent pas être vides !</span>"); }
	// Field must end by an extension
	if (!text.endsWith(".%ext%"))
	{ return QObject::tr("<span style=\"color:orange\">Votre nom de fichier ne finit pas par une extension, symbolisée par %ext% ! Vous risquez de ne pas pouvoir ouvrir vos fichiers.</span>"); }
	// Field must contain an unique token
	if (!text.contains("%md5%"))
	{ return QObject::tr("<span style=\"color:orange\">Votre nom de fichier n'est pas unique à chaque image et une image risque d'en écraser une précédente lors de la sauvegarde ! Vous devriez utiliser le symbole %md5%, unique à chaque image, pour éviter ce désagrément.</span>"); }
	// Looking for unknown tokens
	QStringList tokens = QStringList() << "artist" << "general" << "copyright" << "character" << "model" << "model|artist" << "filename" << "rating" << "md5" << "website" << "ext" << "all";
	QRegExp rx = QRegExp("%(.+)%");
	rx.setMinimal(true);
	int pos = 0;
	while ((pos = rx.indexIn(text, pos)) != -1)
	{
		if (!tokens.contains(rx.cap(1)))
		{ return QObject::tr("<span style=\"color:orange\">Le symbole %%1% n\'existe pas et ne sera pas remplacé.</span>").arg(rx.cap(1)); }
		pos += rx.matchedLength();
	}
	// All tests passed
	return QObject::tr("<span style=\"color:green\">Format valide !</span>");
}

/**
 * Return the path to a specified file in the config folder (since program files is not writable).
 * @param	file	The file.
 * @return			The absolute path to the file.
 */
QString savePath(QString file)
{ return QDir::toNativeSeparators(QDir::homePath()+"/Grabber/"+file); }

/**
 * Opens the explorer and select the file.
 * @param	pathIn	The path to the file.
 */
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
		QDesktopServices::openUrl("file:///"+pathIn);
	#endif
}

QString qfonttocss(QFont font)
{
	QString style;
	switch (font.style())
	{
		case QFont::StyleNormal:	style = "normal";	break;
		case QFont::StyleItalic:	style = "italic";	break;
		case QFont::StyleOblique:	style = "oblique";	break;
	}
	QString size;
	if (font.pixelSize() == -1)
	{ size = QString::number(font.pointSize())+"pt"; }
	else
	{ size = QString::number(font.pixelSize())+"px"; }
	QString weight = QString::number(font.weight()*8); // should be "font.weight()*8+100", but linux doesn't handle weight the same way windows do
	QStringList decorations;
	if (font.strikeOut())	{ decorations.append("line-through");	}
	if (font.underline())	{ decorations.append("underline");		}
	return "font-family:'"+font.family()+"'; font-size:"+size+"; font-style:"+style+"; font-weight:"+weight+"; text-decoration:"+(decorations.isEmpty() ? "none" : decorations.join(" "))+";";
}
