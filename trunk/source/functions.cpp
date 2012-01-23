#include <QApplication>
#include <QtGui>
#include "functions.h"
#include "mainwindow.h"

extern QMap<QDateTime,QString> _log;
extern mainWindow *_mainwindow;



/**
 * Popup a message notifying the user that something went wrong.
 * @param	parent	The parent widget
 * @param	error	The error message
 */
void error(QWidget *parent, QString error)
{ QMessageBox::critical(parent, "Error", error); }

/**
 * Sort a list non case-sensitively.
 * @param	sList	The list that will be ordered
 */
void sortNonCaseSensitive(QStringList &sList)
{
	QMap<QString, QString> strMap;
	foreach (QString str, sList)
	{ strMap.insert( str.toLower(), str); }
	sList = strMap.values();
}

/**
 * Load custom tokens from settings.
 * @return	The map with token names as keys and token tags as values.
 */
QMap<QString,QStringList> getCustoms()
{
	QMap<QString,QStringList> tokens;
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	settings.beginGroup("Save/Customs");
	QStringList keys = settings.childKeys();
	for (int i = 0; i < keys.size(); i++)
	{ tokens.insert(keys.at(i), settings.value(keys.at(i)).toString().split(' ')); }
	return tokens;
}

/**
 * Load multiple filenames from settings.
 * @return	The map with token names as keys and token tags as values.
 */
QMap<QString,QString> getFilenames()
{
	QMap<QString,QString> tokens;
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	settings.beginGroup("Filenames");
	QStringList keys = settings.childKeys();
	for (int i = 0; i < keys.size(); i++)
	{
		if (!keys.at(i).isEmpty())
		{ tokens.insert(keys.at(i), settings.value(keys.at(i)).toString()); }
	}
	return tokens;
}

/**
 * Convert a danbooru-like date (Sat May 14 17:38:04 -0400 2011) to a valid QDateTime.
 * @param	str				The date string.
 * @param	timezonedecay	The number of timezones between the user and the server.
 * @return	The converted date as a QDateTime.
 */
QDateTime qDateTimeFromString(QString str, int timezonedecay)
{
	QDateTime date;
	QList<QString> months;
	months << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" << "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";
	QTime time(str.mid(11,2).toInt(), str.mid(14,2).toInt(), str.mid(17,2).toInt());
	date.setDate(QDate(str.mid(26,4).toInt(), months.indexOf(str.mid(4,3))+1, str.mid(8,2).toInt()+(str.mid(11,2).toInt() >= 18)));
	date.setTime(time.addSecs(3600*timezonedecay));
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
 * Load view it later tags from local file.
 * @return	A QStringList containing tags to view later
 */
QStringList loadViewItLater()
{
	QStringList viewitlater;
	QFile file(savePath("viewitlater.txt"));
	if (file.open(QIODevice::ReadOnly))
	{
		QString vil = file.readAll();
		viewitlater = vil.replace("\r\n", "\n").replace("\r", "\n").split("\n");
		file.close();
	}
	return viewitlater;
}

/**
 * Load ignored tags from local file.
 * @return	A QStringList containing tags
 */
QStringList loadIgnored()
{
	QStringList ignore;
	QFile file(savePath("ignore.txt"));
	if (file.open(QIODevice::ReadOnly))
	{
		QString vil = file.readAll();
		ignore = vil.replace("\r\n", "\n").replace("\r", "\n").split("\n");
		file.close();
	}
	return ignore;
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
	if (!text.contains("%md5%") && !text.contains("%id%"))
	{ return QObject::tr("<span style=\"color:orange\">Votre nom de fichier n'est pas unique à chaque image et une image risque d'en écraser une précédente lors de la sauvegarde ! Vous devriez utiliser le symbole %md5%, unique à chaque image, pour éviter ce désagrément.</span>"); }
	// Looking for unknown tokens
	QStringList tokens = QStringList() << "artist" << "general" << "copyright" << "character" << "model" << "filename" << "rating" << "md5" << "website" << "ext" << "all" << "id" << "search" << "allo" << getCustoms().keys();
	QRegExp rx = QRegExp("%(.+)%");
	rx.setMinimal(true);
	int pos = 0;
	while ((pos = rx.indexIn(text, pos)) != -1)
	{
		if (!tokens.contains(rx.cap(1)) && !rx.cap(1).startsWith("search_"))
		{ return QObject::tr("<span style=\"color:orange\">Le symbole %%1% n\'existe pas et ne sera pas remplacé.</span>").arg(rx.cap(1)); }
		pos += rx.matchedLength();
	}
	// All tests passed
	if (!text.contains("%md5%") && !text.contains("%website%"))
	{ return QObject::tr("<span style=\"color:green\">Vous avez choisi d'utiliser le symbole %id%. Sachez que celui-ci est unique pour un site choisi. Le même ID pourra identifier des images différentes en fonction du site.</span>"); }
	return QObject::tr("<span style=\"color:green\">Format valide !</span>");
}

/**
 * Return the path to a specified file in the config folder (since program files is not writable).
 * @param	file	The file.
 * @return			The absolute path to the file.
 */
QString savePath(QString file)
{
	if (QFile(QDir::toNativeSeparators(qApp->applicationDirPath()+"/settings.ini")).exists())
	{ return QDir::toNativeSeparators(qApp->applicationDirPath()+"/"+file); }
	return QDir::toNativeSeparators(QDir::homePath()+"/Grabber/"+file);
}

/**
 * Return the levenshtein distance between two strings.
 * @param	s1	First string.
 * @param	s2	Second string.
 * @return		The levenshtein distance between s1 and s2.
 */
int levenshtein(QString s1, QString s2)
{
	const size_t len1 = s1.size(), len2 = s2.size();
	vector<vector<unsigned int> > d(len1 + 1, vector<unsigned int>(len2 + 1));

	d[0][0] = 0;
	for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
	for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

	for(unsigned int i = 1; i <= len1; ++i)
		for(unsigned int j = 1; j <= len2; ++j)
			d[i][j] = std::min( std::min(d[i - 1][j] + 1,d[i][j - 1] + 1),
				d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) );

	return d[len1][len2];
}

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

/**
 * Converts a QFont to a CSS string.
 * @param	font	The font to convert.
 * @return	The CSS font.
 */
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

/**
 * Converts a DOM elemet to a map.
 * @param	dom		The DOM element to convert.
 * @return	A QString map with names (joined with a slash if necessary) as keys and texts as values.
 */
QMap<QString,QString> domToMap(QDomElement dom)
{
	QMap<QString,QString> details;
	dom.firstChildElement("Name").firstChild().nodeValue();
	for (QDomNode n = dom.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if (n.firstChild().nodeName() == "#text")
		{ details[n.nodeName()] = n.firstChild().nodeValue(); }
		else
		{
			QMap<QString,QString> r = domToMap(n.toElement());
			QStringList k = r.keys();
			for (int i = 0; i < r.count(); i++)
			{ details[n.nodeName()+"/"+k.at(i)] = r.value(k.at(i)); }
		}
	}
	return details;
}

/**
 * Append text in the log in a new line.
 * @param	l	The message to append.
 */
void log(QString l, Log type)
{
	qDebug() << l;
	QDateTime time = QDateTime::currentDateTime();
	_log.insert(time, (type == Error ? QObject::tr("<b>Erreur :</b> %1").arg(l) : (type == Warning ? QObject::tr("<b>Attention :</b> %1").arg(l) : (type == Notice ? QObject::tr("<b>Notice :</b> %1").arg(l) : l))));

	QFile f("main.log");
	if (f.open(QFile::Append | QFile::Text | (_log.count() == 1 ? QFile::Truncate : QFile::NotOpen)))
	{ f.write(QString("["+time.toString("hh:mm:ss.zzz")+"] "+l+"\r\n").toAscii()); }
	f.close();

	_mainwindow->logShow();
}

/**
 * Append text in the log at the end of the current line.
 * @param	l	The message to append.
 */
void logUpdate(QString l)
{
	qDebug() << l;
	QDateTime date = _log.keys().at(_log.count()-1);
	QString message = _log.value(date)+l;
	_log.insert(date, message);
	_mainwindow->logShow();
}
