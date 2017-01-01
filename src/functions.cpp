#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include "math.h"
#include "functions.h"
#ifdef Q_OS_WIN
	#include <windows.h>
#else
	#include <utime.h>
#endif
#ifdef QT_DEBUG
	#include <QDebug>
#endif
#include "mainwindow.h"

using namespace std;

extern mainWindow *_mainwindow;



/**
 * Popup a message notifying the user that something went wrong.
 * @param	parent	The parent widget
 * @param	error	The error message
 */
void error(QWidget *parent, QString error)
{
	#ifndef TEST
		QMessageBox::critical(parent, QObject::tr("Error"), error);
	#else
		Q_UNUSED(parent);
		Q_UNUSED(error);
	#endif
}

/**
 * Log SSL errors in debug mode only.
 *
 * @param qnr		The network reply who generated the SSL errors
 * @param errors	The list of SSL errors that occured
 */
void sslErrorHandler(QNetworkReply* qnr, QList<QSslError> errors)
{
	#ifdef QT_DEBUG
		qDebug() << errors;
	#else
		Q_UNUSED(errors);
	#endif
	#ifndef TEST
		qnr->ignoreSslErrors();
	#else
		Q_UNUSED(qnr);
	#endif
}

/**
 * Load custom tokens from settings.
 * @return	The map with token names as keys and token tags as values.
 */
QMap<QString, QStringList> getCustoms(QSettings *settings)
{
	QMap<QString,QStringList> tokens;
	settings->beginGroup("Save/Customs");
	QStringList keys = settings->childKeys();
	for (int i = 0; i < keys.size(); i++)
	{ tokens.insert(keys.at(i), settings->value(keys.at(i)).toString().split(' ')); }
	settings->endGroup();
	return tokens;
}

/**
 * Load multiple filenames from settings.
 * @return	The map with token names as keys and token tags as values.
 */
QMap<QString,QPair<QString,QString>> getFilenames(QSettings *settings)
{
	QMap<QString,QPair<QString,QString>> tokens;

	settings->beginGroup("Filenames");
	int count = settings->childKeys().count() / 3;
	for (int i = 0; i < count; i++)
	{
		if (settings->contains(QString::number(i) + "_cond"))
		{
			QPair<QString,QString> pair;
			pair.first = settings->value(QString::number(i) + "_fn").toString();
			pair.second = settings->value(QString::number(i) + "_dir").toString();
			tokens.insert(settings->value(QString::number(i) + "_cond").toString(), pair);
		}
	}
	settings->endGroup();

	return tokens;
}

/**
 * Convert a danbooru-like date (Sat May 14 17:38:04 -0400 2011) to a valid QDateTime.
 * @param	str				The date string.
 * @return	The converted date as a QDateTime.
 */
QDateTime qDateTimeFromString(QString str)
{
	int timezone = QDateTime::currentDateTime().time().hour() - QDateTime::currentDateTimeUtc().time().hour();

	QDateTime date;
	if (str[0].isDigit())
	{
		float decay = 0;
		date = QDateTime::fromString(str.left(19), "yyyy-MM-dd'T'HH:mm:ss");
		if (!date.isValid())
			date = QDateTime::fromString(str.left(19), "yyyy/MM/dd HH:mm:ss");
		else
			decay = str.right(6).remove(':').toFloat() / 100;

		if (date.isValid())
			date = date.addSecs(3600 * (timezone - decay));
	}
	else
	{
		QStringList months = QStringList() << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" << "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";
		int year = str.mid(26, 4).toInt();
		int month = months.indexOf(str.mid(4, 3)) + 1;
		int day = str.mid(8, 2).toInt();
		float decay = str.mid(20, 5).toFloat() / 100;

		QTime time = QTime::fromString(str.mid(11, 8), "HH:mm:ss");
		time = time.addSecs(3600 * (timezone - decay));
		date.setDate(QDate(year, month, day));
		date.setTime(time);
	}
	return date;
}

QString getUnit(float *value)
{
	QString unit = "o";
	if (*value >= 1024)
	{
		*value /= 1024;
		if (*value >= 1024)
		{
			*value /= 1024;
			unit = "Mio";
		}
		else
		{ unit = "Kio"; }
	}
	return unit;
}

/**
 * Return the path to a specified file in the config folder (since program files is not writable).
 * @param	file	The file.
 * @param	exists	If the file must already exist beforehand.
 * @return			The absolute path to the file.
 */
QString savePath(QString file, bool exists)
{
	#ifdef TEST
		Q_UNUSED(exists);
		return QDir::toNativeSeparators(QDir::currentPath()+"/tests/resources/"+file);
	#else
		QString check = exists ? file : "settings.ini";
		if (QFile(QDir::toNativeSeparators(qApp->applicationDirPath()+"/"+check)).exists())
		{ return QDir::toNativeSeparators(qApp->applicationDirPath()+"/"+file); }
		if (QFile(QDir::toNativeSeparators(QDir::currentPath()+"/"+check)).exists())
		{ return QDir::toNativeSeparators(QDir::currentPath()+"/"+file); }
		if (QFile(QDir::toNativeSeparators(QDir::homePath()+"/Grabber/"+check)).exists())
		{ return QDir::toNativeSeparators(QDir::homePath()+"/Grabber/"+file); }
		#ifdef __linux__
			if (QFile(QDir::toNativeSeparators(QDir::homePath()+"/.Grabber/"+check)).exists())
			{ return QDir::toNativeSeparators(QDir::homePath()+"/.Grabber/"+file); }
		#endif
		return QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/"+file);
	#endif
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
	QVector<QVector<unsigned int> > d(len1 + 1, QVector<unsigned int>(len2 + 1));

	d[0][0] = 0;
	for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
	for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

	for(unsigned int i = 1; i <= len1; ++i)
		for (unsigned int j = 1; j <= len2; ++j)
			d[i][j] = qMin(
				qMin(
					d[i - 1][j] + 1,
					d[i][j - 1] + 1
				),
				d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1)
			);

	return d[len1][len2];
}

bool setFileCreationDate(QString path, QDateTime datetime)
{
	#ifdef Q_OS_WIN
		LPCWSTR filename = (const wchar_t*)path.utf16();
		HANDLE hfile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hfile == INVALID_HANDLE_VALUE)
		{
			log(QString("Unable to open file (%1)").arg(GetLastError()), Log::Error);
			return false;
		}
		else
		{
			LONGLONG ll = Int32x32To64(datetime.toTime_t(), 10000000) + 116444736000000000;
			FILETIME pcreationtime;
			pcreationtime.dwLowDateTime = (DWORD) ll;
			pcreationtime.dwHighDateTime = ll >> 32;

			if (!SetFileTime(hfile, &pcreationtime, NULL, &pcreationtime))
			{
				log(QString("Unable to change the file creation date (%1)").arg(GetLastError()), Log::Error);
				return false;
			}
		}
		CloseHandle(hfile);
	#else
		struct utimbuf timebuffer;
		timebuffer.modtime = datetime.toTime_t();
		const char *filename = path.toStdString().c_str();
		if ((utime(filename, &timebuffer)) < 0)
		{
			// log(QString("Unable to change the file creation date (%d)").arg(errno), Log::Error);
			return false;
		}
	#endif
	return true;
}

/**
 * Opens the file explorer and select the file.
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
		// Q_UNUSED(parent)
		QStringList scriptArgs;
		scriptArgs << QLatin1String("-e") << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(pathIn);
		QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
		scriptArgs.clear();
		scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
		QProcess::execute("/usr/bin/osascript", scriptArgs);
	#else
		QDesktopServices::openUrl(QUrl("file:///"+pathIn));
	#endif
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
QFile g_logFile, fCommandsLog, fCommandsSqlLog;
void setLogFile(QString path)
{
	g_logFile.setFileName(path);
	g_logFile.open(QFile::Append | QFile::Text | QFile::Truncate);
}

void log(QString l, Log type)
{
	if (!g_logFile.isOpen())
		setLogFile(savePath("main.log"));

	QDateTime time = QDateTime::currentDateTime();
	g_logFile.write(QString("["+time.toString("hh:mm:ss.zzz")+"] "+stripTags(l)+"\n").toUtf8());
	g_logFile.flush();

	#ifndef TEST
		QString msg = (type == Error ? QObject::tr("<b>Error:</b> %1").arg(l) : (type == Warning ? QObject::tr("<b>Warning:</b> %1").arg(l) : (type == Notice ? QObject::tr("<b>Notice:</b> %1").arg(l) : l)));
		_mainwindow->logShow(time, msg);
    #else
        Q_UNUSED(type);
	#endif

	#ifdef QT_DEBUG
		#ifndef TEST
			qDebug() << time.toString("hh:mm:ss.zzz") << l;
		#endif
	#endif
}
void logCommand(QString l)
{
	if (!fCommandsLog.isOpen())
	{
		fCommandsLog.setFileName(savePath("commands.log"));
		fCommandsLog.open(QFile::Append | QFile::Text | QFile::Truncate);
	}

	fCommandsLog.write(QString(l+"\r\n").toUtf8());
	fCommandsLog.flush();
}
void logCommandSql(QString l)
{
	if (!fCommandsSqlLog.isOpen())
	{
		fCommandsSqlLog.setFileName(savePath("commands.sql"));
		fCommandsSqlLog.open(QFile::Append | QFile::Text | QFile::Truncate);
	}

	fCommandsSqlLog.write(QString(l+"\r\n").toUtf8());
	fCommandsSqlLog.flush();
}

/**
 * Removes HTML from a string.
 * @param	str		The string to remove HTML from.
 * @return			The string without html.
 */
QString stripTags(QString str)
{ return str.remove(QRegExp("<[^>]*>")); }

/**
 * Append text in the log at the end of the current line.
 * @param	l	The message to append.
 */
void logUpdate(QString l)
{
	Q_UNUSED(l);

	/*QDateTime date = _log.keys().at(_log.count()-1);
	QString message = _log.value(date)+l;
	_log.insert(date, message);
	_mainwindow->logShow();

	qDebug() << l;*/
}

/**
 * Shut down computer after a certain period of time.
 * @param	timeout		Time before shutdown in seconds.
 */
void shutDown(int timeout)
{
	#if defined(Q_OS_WIN)
		QProcess::startDetached("shutdown -s -f -t "+QString::number(timeout));
	#else
		QProcess::startDetached("shutdown "+QString::number(timeout));
	#endif
}

/**
 * Opens CD tray.
 */
void openTray()
{
	#if defined(Q_OS_WIN)
		QProcess::startDetached("CDR.exe open");
	#else
		QProcess::startDetached("eject cdrom");
	#endif
}

void clearLayout(QLayout *layout)
{
	QLayoutItem *item;
	while ((item = layout->takeAt(0)))
	{
		if (item->layout())
		{
			clearLayout(item->layout());
			item->layout()->deleteLater();
		}
		item->widget()->deleteLater();
		delete item;
	}
}

QString getExtension(QUrl url)
{ return getExtension(url.toString()); }
QString getExtension(QString url)
{
	QString ext;
	int pPos = url.lastIndexOf('.');
	if (pPos != -1 && pPos > url.indexOf('/', 7))
	{
		ext = url.right(url.length() - pPos - 1);
		if (ext.contains('?'))
			ext = ext.section('?', 0, -2);
	}
	return ext;
}

QString setExtension(QString url, QString extension)
{
	int pPos = url.lastIndexOf('.');
	if (pPos != -1 && pPos > url.indexOf('/', 7))
	{
		int qPos = url.indexOf('?', pPos);
		if (qPos != -1)
			url.replace(pPos + 1, qPos - pPos - 1, extension);
		else
			url = url.left(pPos) + "." + extension;
	}
	return url;
}

QString fixFilename(QString fn, QString path, int maxlength, bool invalidChars)
{
	QString sep = QDir::toNativeSeparators("/");
	fn = QDir::toNativeSeparators(fn);
	path = QDir::toNativeSeparators(path);
	if (!path.endsWith(sep) && !path.isEmpty() && !fn.isEmpty())
		path += sep;

	#ifdef Q_OS_WIN
		return fixFilenameWindows(fn, path, maxlength, invalidChars);
	#else
		return fixFilenameLinux(fn, path, maxlength, invalidChars);
	#endif
}

QString fixFilenameLinux(QString fn, QString path, int maxlength, bool invalidChars)
{
	Q_UNUSED(invalidChars);

	// Fix parameters
	QString sep = "/";
	QString filename = path + fn;

	// Divide filename
	QStringList parts = filename.split(sep);
	QString file, ext;
	if (!fn.isEmpty())
	{
		file = parts.takeLast();;
		int lastDot = file.lastIndexOf('.');
		if (lastDot != -1)
		{
			ext = file.right(file.length() - lastDot - 1);
			file = file.left(lastDot);
		}
	}

	// Fix directories
	for (QString &part : parts)
	{
		// A part cannot start or finish with a space
		part = part.trimmed();

		// Trim part
		if (part.length() > 255)
			part = part.left(255).trimmed();
	}

	// Join parts back
	QString dirpart = parts.join(sep);
	filename = (dirpart.isEmpty() ? "" : dirpart + (!fn.isEmpty() ? sep : "")) + file;

	// A filename cannot exceed a certain length
	int extlen = ext.isEmpty() ? 0 : ext.length() + 1;
	if (file.length() > maxlength - extlen)
		file = file.left(maxlength - extlen).trimmed();
	if (file.length() > 255 - extlen)
		file = file.left(255 - extlen).trimmed();

	// Get separation between filename and path
	int index = -1;
	int pathGroups = path.count(sep);
	for (int i = 0; i < pathGroups; ++i)
		index = filename.indexOf(sep, index + 1);

	// Put extension and drive back
	filename = filename + (!ext.isEmpty() ? "." + ext : "");
	if (!fn.isEmpty())
		filename = filename.right(filename.length() - index - 1);

	QFileInfo fi(filename);
	QString suffix = fi.suffix();
	filename = (fi.path() != "." ? fi.path() + "/" : "") + fi.completeBaseName().left(245) + (suffix.isEmpty() ? "" : "." + fi.suffix());

	return filename;
}

#ifndef MAX_PATH
	#define MAX_PATH 260
#endif

QString fixFilenameWindows(QString fn, QString path, int maxlength, bool invalidChars)
{
	// Fix parameters
	QString sep = "\\";
	maxlength = maxlength == 0 ? MAX_PATH : maxlength;
	QString filename = path + fn;

	// Drive
	QString drive = "";
	if (filename.mid(1, 2) == ":\\")
	{
		drive = filename.left(3);
		filename = filename.right(filename.length() - 3);
	}

	// Forbidden characters
	if (invalidChars)
	{ filename.replace('<', '_').replace('>', '_').replace(':', '_').remove('"').replace('/', '_').replace('|', '_').remove('?').replace('*', '_'); }

	// Fobidden directories or filenames
	QStringList forbidden = QStringList() << "CON" << "PRN" << "AUX" << "NUL" << "COM1" << "COM2" << "COM3" << "COM4" << "COM5" << "COM6" << "COM7" << "COM8" << "COM9" << "LPT1" << "LPT2" << "LPT3" << "LPT4" << "LPT5" << "LPT6" << "LPT7" << "LPT8" << "LPT9";

	// Divide filename
	QStringList parts = filename.split(sep);
	QString file, ext;
	if (!fn.isEmpty())
	{
		file = parts.takeLast();
		int lastDot = file.lastIndexOf('.');
		if (lastDot != -1)
		{
			ext = file.right(file.length() - lastDot - 1);
			file = file.left(lastDot);
		}
	}

	// Fix directories
	for (QString &part : parts)
	{
		// A part cannot be one in the forbidden list
		if (invalidChars && forbidden.contains(part))
		{ part = part + "!"; }

		// A part cannot finish by a period
		if (invalidChars && part.endsWith('.'))
		{ part = part.left(part.length() - 1).trimmed(); }

		// A part cannot start or finish with a space
		part = part.trimmed();

		// A part should still allow creating a file
		if (part.length() > maxlength - 12)
		{ part = part.left(qMax(0, maxlength - 12)).trimmed(); }
	}

	// Join parts back
	QString dirpart = parts.join(sep);
	if (dirpart.length() > maxlength - 12)
	{ dirpart = dirpart.left(qMax(0, maxlength - 12)).trimmed(); }
	filename = (dirpart.isEmpty() ? "" : dirpart + (!fn.isEmpty() ? sep : "")) + file;

	// A filename cannot exceed MAX_PATH (-1 for <NUL> and -3 for drive "C:\")
	if (filename.length() > maxlength - 1 - 3 - ext.length() - 1)
	{ filename = filename.left(qMax(0, maxlength - 1 - 3 - ext.length() - 1)).trimmed(); }

	// Get separation between filename and path
	int index = -1;
	int pathGroups = path.count(sep);
	for (int i = 0; i < pathGroups - (!drive.isEmpty() ? 1 : 0); ++i)
	{ index = filename.indexOf(sep, index + 1); }
	index += drive.length();

	// Put extension and drive back
	filename = drive + filename + (!ext.isEmpty() ? "." + ext : "");
	if (!fn.isEmpty())
	{ filename = filename.right(filename.length() - index - 1); }

	return filename;
}
