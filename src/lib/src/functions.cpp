#include "functions.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QJSValue>
#include <QLocale>
#include <QProcess>
#include <QRect>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QTime>
#include <QtMath>
#include <QUrl>
#include <QVector>
#ifdef Q_OS_WIN
	#include <Windows.h>
	#include <winbase.h>
#else
	#include <errno.h>
	#include <unistd.h>
	#include <utime.h>
#endif
#ifdef QT_DEBUG
	#include <QDebug>
#endif
#include "filename/conditional-filename.h"
#include "logger.h"
#include "vendor/html-entities.h"


int lastError()
{
	#ifdef Q_OS_WIN
		return GetLastError();
	#else
		return errno;
	#endif
}

QString lastErrorString()
{
	const int errorCode = lastError();
	if (errorCode == 0) {
		return QString();
	}

	#ifdef Q_OS_WIN
		// https://stackoverflow.com/a/17387176/828828
		LPWSTR messageBuffer = nullptr;
		size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &messageBuffer, 0, NULL);
		auto message = QString::fromWCharArray(messageBuffer, size);
		LocalFree(messageBuffer);
		return message;
	#else
		return strerror(errorCode);
	#endif
}


QStringList splitCommand(const QString &command)
{
	QStringList args;

	QString tmp;
	int quoteCount = 0;
	QChar lastQuote;
	bool inQuote = false;
	bool inEscape = false;

	for (const QChar c : command) {
		// Escape character (\)
		if (c == QLatin1Char('\\') && !inEscape) {
			inEscape = true;
			continue;
		}
		if (inEscape) {
			inEscape = false;
			tmp += c;
			continue;
		}

		// Count quotes
		if ((c == QLatin1Char('"') || c == QLatin1Char('\'')) && (c == lastQuote || lastQuote.isNull())) {
			++quoteCount;
			lastQuote = c;

			// Allow escaping quotes using triple quotes
			if (quoteCount == 3) {
				quoteCount = 0;
				tmp += c;
			}
			continue;
		}

		if (quoteCount) {
			// If the previous character was only one quote
			if (quoteCount == 1) {
				inQuote = !inQuote;
				if (!inQuote) {
					lastQuote = QChar();
				}
			}

			// This means we saw two quotes in a row, which are ignored
			quoteCount = 0;
		}

		// If we finally reached a space outside a quoted argument, we flush
		if (!inQuote && c.isSpace()) {
			args += tmp;
			tmp.clear();
		} else {
			tmp += c;
		}
	}

	// Flush the last argument
	if (!tmp.isEmpty()) {
		args += tmp;
	}

	return args;
}

QStringList splitStringMulti(const QList<QChar> &seps, const QString &str, bool skipEmpty)
{
	QStringList ret;

	int begin = 0;
	for (int i = 0; i < str.length(); ++i) {
		if (seps.indexOf(str[i]) != -1) {
			if (i - begin > 0 || !skipEmpty) {
				ret.append(str.mid(begin, i - begin));
			}
			begin = i + 1;
		}
	}
	if (begin < str.count() || !skipEmpty) {
		ret.append(str.mid(begin));
	}
	return ret;
}


/**
 * Load custom tokens from settings.
 * @return	The map with token names as keys and token tags as values.
 */
QMap<QString, QStringList> getCustoms(QSettings *settings)
{
	QMap<QString, QStringList> tokens;
	settings->beginGroup(QStringLiteral("Save/Customs"));
	const QStringList keys = settings->childKeys();
	for (const QString &key : keys) {
		tokens.insert(key, splitStringMulti({ ' ', '\n' }, settings->value(key).toString(), true));
	}
	settings->endGroup();
	return tokens;
}

/**
 * Load multiple filenames from settings.
 * @return	The map with token names as keys and token tags as values.
 */
QList<ConditionalFilename> getConditionalFilenames(QSettings *settings)
{
	QList<ConditionalFilename> ret;

	settings->beginGroup(QStringLiteral("Filenames"));
	const int count = settings->childKeys().count() / 3;
	for (int i = 0; i < count; i++) {
		const QString strI = QString::number(i);
		if (settings->contains(strI + "_cond")) {
			ret.append(ConditionalFilename(
				settings->value(strI + "_cond").toString(),
				settings->value(strI + "_fn").toString(),
				settings->value(strI + "_dir").toString()
			));
		}
	}
	settings->endGroup();

	return ret;
}

QMap<int, QMap<QString, QVariant>> getExternalLogFiles(QSettings *settings)
{
	QMap<int, QMap<QString, QVariant>> ret;

	settings->beginGroup(QStringLiteral("LogFiles"));
	for (const QString &group : settings->childGroups()) {
		settings->beginGroup(group);
		QMap<QString, QVariant> logSettings;
		for (const QString &key : settings->childKeys()) {
			logSettings.insert(key, settings->value(key));
		}
		ret.insert(group.toInt(), logSettings);
		settings->endGroup();
	}
	settings->endGroup();

	return ret;
}
QStringList getExternalLogFilesSuffixes(QSettings *settings)
{
	QStringList suffixes;

	auto logFiles = getExternalLogFiles(settings);
	for (auto it = logFiles.constBegin(); it != logFiles.constEnd(); ++it) {
		const QMap<QString, QVariant> &logFile = it.value();
		if (logFile["locationType"].toInt() == 2) {
			suffixes.append(logFile["suffix"].toString());
		}
	}

	return suffixes;
}

QList<QPair<QString, QString>> getMetadata(QSettings *settings, const QString &arrayKey)
{
	QList<QPair<QString, QString>> ret;

	const int size = settings->beginReadArray(arrayKey);
	for (int i = 0; i < size; ++i) {
		settings->setArrayIndex(i);
		const QString key = settings->value("key").toString();
		const QString value = settings->value("value").toString();
		ret.append(QPair<QString, QString> { key, value });
	}
	settings->endArray();

	return ret;
}
QList<QPair<QString, QString>> getMetadataPropsys(QSettings *settings)
{
	return getMetadata(settings, "Save/MetadataPropsys");
}
QList<QPair<QString, QString>> getMetadataExiftool(QSettings *settings)
{
	return getMetadata(settings, "Save/MetadataExiftool");
}

QStringList removeWildards(const QStringList &elements, const QStringList &remove)
{
	QStringList tags;

	for (const QString &tag : elements) {
		bool removed = false;
		for (const QString &rem : remove) {
			const auto reg = QRegularExpression::fromWildcard(rem, Qt::CaseInsensitive);
			if (reg.match(tag).hasMatch()) {
				removed = true;
				break;
			}
		}

		if (!removed) {
			tags.append(tag);
		}
	}

	return tags;
}

/**
 * Convert a danbooru-like date (Sat May 14 17:38:04 -0400 2011) to a valid QDateTime.
 * @param	str				The date string.
 * @return	The converted date as a QDateTime.
 */
QDateTime qDateTimeFromString(const QString &str)
{
	QDateTime date;

	// UTC timestamp
	const uint toInt = str.toUInt();
	if (toInt != 0) {
		return QDateTime::fromSecsSinceEpoch(toInt, Qt::UTC);
	}

	if ((str.length() == 23 || str.length() == 26) && str[19] == '.') {
		date = QDateTime::fromString(str.left(23), QStringLiteral("yyyy/MM/dd HH:mm:ss.zzz"));
		if (!date.isValid()) {
			date = QDateTime::fromString(str.left(23), QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
		}
		date.setTimeSpec(Qt::UTC);
	} else if (str.length() == 19) {
		date = QDateTime::fromString(str, QStringLiteral("yyyy/MM/dd HH:mm:ss"));
		if (!date.isValid()) {
			date = QDateTime::fromString(str, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
		}
		date.setTimeSpec(Qt::UTC);
	} else if (str.length() == 16) {
		date = QDateTime::fromString(str, QStringLiteral("yyyy/MM/dd HH:mm"));
		if (!date.isValid()) {
			date = QDateTime::fromString(str, QStringLiteral("yyyy-MM-dd HH:mm"));
		}
		date.setTimeSpec(Qt::UTC);
	} else if (str[0].isDigit()) {
		qreal decay = 0;

		date = QDateTime::fromString(str.left(19), QStringLiteral("yyyy-MM-dd'T'HH:mm:ss"));
		if (!date.isValid()) {
			date = QDateTime::fromString(str.left(19), QStringLiteral("yyyy/MM/dd HH:mm:ss"));
		} else {
			decay = str.right(6).remove(':').toDouble() / 100;
		}
		date.setOffsetFromUtc(qFloor(3600 * decay));
	} else {
		QLocale myLoc(QLocale::English);
		date = myLoc.toDateTime(str, QStringLiteral("ddd MMM dd HH:mm:ss yyyy"));
		if (!date.isValid()) {
			date = myLoc.toDateTime(str, QStringLiteral("ddd MMM  d HH:mm:ss yyyy"));
		}
		if (date.isValid()) {
			date.setTimeSpec(Qt::UTC);
			return date;
		}

		const QStringList months { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		const int year = str.mid(26, 4).toInt();
		const int month = months.indexOf(str.mid(4, 3)) + 1;
		const int day = str.mid(8, 2).toInt();
		const qreal decay = str.mid(20, 5).toDouble() / 100;

		const QTime time = QTime::fromString(str.mid(11, 8), QStringLiteral("HH:mm:ss"));
		date.setDate(QDate(year, month, day));
		date.setTime(time);
		date.setOffsetFromUtc(qFloor(3600 * decay));
	}

	return date;
}

QString getUnit(double *size)
{
	QStringList units = FILESIZE_UNITS;
	const int multiplier = FILESIZE_MULTIPLIER;

	int power = 0;
	while (*size >= multiplier && power < units.count() - 1) {
		*size /= 1024;
		power++;
	}

	return units[power];
}

QString formatFilesize(double size)
{
	const QString unit = getUnit(&size);
	const double round = size > 100 ? 1 : (size >= 10 ? 10 : 100);
	const double roundedSize = qRound(size * round) / round;
	return QStringLiteral("%1 %2").arg(roundedSize).arg(unit);
}

qint64 parseFileSize(const QString &str)
{
	static const QRegularExpression rx(QStringLiteral("^(\\d+(?:\\.\\d+)?)\\s*([a-zA-Z]+)$"));
	const auto match = rx.match(str);
	if (match.hasMatch()) {
		const double val = match.captured(1).toDouble();
		const QString unit = match.captured(2).toLower();
		if (unit == QStringLiteral("gb")) {
			return qRound64(val * 1024 * 1024 * 1024);
		}
		if (unit == QStringLiteral("mb")) {
			return qRound64(val * 1024 * 1024);
		}
		if (unit == QStringLiteral("kb")) {
			return qRound64(val * 1024);
		}
		return qRound64(val);
	}
	return qRound64(str.toDouble());
}

bool validSavePath(const QString &file, bool writable)
{
	QString nativeFile = QDir::toNativeSeparators(file);
	QFileInfo info(nativeFile);
	const bool isWritable = info.isWritable() && !nativeFile.startsWith(QLatin1String("C:\\Program Files"));
	return info.exists() && (!writable || isWritable);
}

/**
 * Return the path to a specified file in the config folder (since program files is not writable).
 * @param	file	The file.
 * @param	exists	If the file must already exist beforehand.
 * @return			The absolute path to the file.
 */
QString savePath(const QString &file, bool exists, bool writable)
{
	const QString &check = exists ? file : QStringLiteral("settings.ini");

	// Test mode
	if (isTestModeEnabled()) {
		if (QDir(QDir::currentPath() + "/tests/resources/").exists()) {
			return QDir::toNativeSeparators(QDir::currentPath() + "/tests/resources/" + file);
		}
	}

	// AppImages should read from their embedded filesystem even when running in portable mode
	#ifdef __linux__
		if (QCoreApplication::applicationFilePath().endsWith(".AppImage")) {
			if (validSavePath(qApp->applicationDirPath() + "/../share/Grabber/" + check, writable)) {
				return QDir::toNativeSeparators(qApp->applicationDirPath() + "/../share/Grabber/" + file);
			}
		}
	#endif

	// Install directory and portable mode
	if (validSavePath(qApp->applicationDirPath() + "/" + check, writable)) {
		return QDir::toNativeSeparators(qApp->applicationDirPath() + "/" + file);
	}
	#if defined(Q_OS_ANDROID)
		if (validSavePath("assets:/" + check, writable)) {
			return QDir::toNativeSeparators("assets:/" + file);
		}
	#endif

	// Various possible configuration directories
	if (validSavePath(QDir::currentPath() + "/" + check, writable)) {
		return QDir::toNativeSeparators(QDir::currentPath() + "/" + file);
	}
	if (validSavePath(QDir::homePath() + "/Grabber/" + check, writable)) {
		return QDir::toNativeSeparators(QDir::homePath() + "/Grabber/" + file);
	}
	#ifdef __linux__
		if (validSavePath(QDir::homePath() + "/.Grabber/" + check, writable)) {
			return QDir::toNativeSeparators(QDir::homePath() + "/.Grabber/" + file);
		}
		if (validSavePath(qApp->applicationDirPath() + "/../share/Grabber/" + check, writable)) {
			return QDir::toNativeSeparators(qApp->applicationDirPath() + "/../share/Grabber/" + file);
		}
		if (validSavePath(QString(PREFIX) + "/share/Grabber/" + check, writable)) {
			return QDir::toNativeSeparators(QString(PREFIX) + "/share/Grabber/" + file);
		}
	#endif

	// Fallback to standard config location if no other exists
	QString dir;
	#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
		#if defined(Q_OS_ANDROID)
			dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
		#else
			dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
		#endif
	#else
		dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
		#ifdef __linux__
			dir += QLatin1Char('/') + QCoreApplication::organizationName();
			dir += QLatin1Char('/') + QCoreApplication::applicationName();
		#endif
	#endif
	return QDir::toNativeSeparators(dir + QLatin1Char('/') + file);
}

/**
 * Return the levenshtein distance between two strings.
 * @param	s1	First string.
 * @param	s2	Second string.
 * @return		The levenshtein distance between s1 and s2.
 */
int levenshtein(QString s1, QString s2)
{
	const int len1 = s1.size(), len2 = s2.size();
	QVector<QVector<int>> d(len1 + 1, QVector<int>(len2 + 1));

	d[0][0] = 0;
	for (int i = 1; i <= len1; ++i) {
		d[i][0] = i;
	}
	for (int i = 1; i <= len2; ++i) {
		d[0][i] = i;
	}

	for (int i = 1; i <= len1; ++i) {
		for (int j = 1; j <= len2; ++j) {
			const int a = qMin(d[i - 1][j] + 1, d[i][j - 1] + 1);
			const int b = d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1);
			d[i][j] = qMin(a, b);
		}
	}

	return d[len1][len2];
}

#ifdef Q_OS_WIN
	wchar_t *toWCharT(const QString &str)
	{
		auto *out = new wchar_t[str.length() + 1];
		str.toWCharArray(out);
		out[str.length()] = 0;
		return out;
	}
#endif

bool setFileCreationDate(const QString &path, const QDateTime &datetime)
{
	if (!datetime.isValid()) {
		return false;
	}
	#ifdef Q_OS_WIN
		wchar_t *filename = toWCharT(path);
		HANDLE hfile = CreateFileW(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		delete[] filename;
		if (hfile == INVALID_HANDLE_VALUE) {
			log(QStringLiteral("Unable to open file to set creation date (%1 - %2): %3").arg(lastError()).arg(lastErrorString(), path), Logger::Error);
			return false;
		}

		const LONGLONG ll = Int32x32To64(datetime.toSecsSinceEpoch(), 10000000) + 116444736000000000;
		FILETIME pcreationtime;
		pcreationtime.dwLowDateTime = static_cast<DWORD>(ll);
		pcreationtime.dwHighDateTime = ll >> 32;

		if (SetFileTime(hfile, &pcreationtime, nullptr, &pcreationtime) == FALSE) {
			log(QStringLiteral("Unable to change the file creation date (%1 - %2): %3").arg(lastError()).arg(lastErrorString(), path), Logger::Error);
			return false;
		}

		CloseHandle(hfile);
	#else
		struct utimbuf timebuffer;
		timebuffer.modtime = datetime.toSecsSinceEpoch();
		timebuffer.actime = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
		if ((utime(path.toStdString().c_str(), &timebuffer)) < 0) {
			log(QStringLiteral("Unable to change the file creation date (%1 - %2): %3").arg(lastError()).arg(lastErrorString(), path), Logger::Error);
			return false;
		}
	#endif
	return true;
}

/**
 * Removes HTML from a string.
 * @param	str		The string to remove HTML from.
 * @return			The string without html.
 */
QString stripTags(QString str)
{
	static const QRegularExpression strip(QStringLiteral("<[^>]*>"));
	return str.remove(strip);
}

/**
 * Shut down computer after a certain period of time.
 * @param	timeout		Time before shutdown in seconds.
 */
void shutDown(int timeout)
{
	#if !defined(QT_NO_PROCESS)
		#if defined(Q_OS_WIN)
			QProcess::startDetached(QString("shutdown"), QStringList({"-s", "-f", "-t", QString::number(timeout)}));
		#else
			QProcess::startDetached(QString("shutdown"), QStringList({QString::number(timeout)}));
		#endif
	#endif
}

/**
 * Opens CD tray.
 */
void openTray()
{
	#if !defined(QT_NO_PROCESS)
		#if defined(Q_OS_WIN)
			QProcess::startDetached(QString("CDR.exe"), QStringList({"open"}));
		#else
			QProcess::startDetached(QString("eject"), QStringList({"cdrom"}));
		#endif
	#endif
}

QString getExtension(const QString &url)
{
	const int lastDot = url.lastIndexOf('.');
	if (lastDot != -1) {
		const int doubleDot = url.mid(lastDot + 1).indexOf(':');
		if (doubleDot != -1) {
			return url.mid(lastDot + 1, doubleDot);
		} else {
			return url.mid(lastDot + 1);
		}
	}
	return QString();
}

QString getExtension(const QUrl &url)
{
	const QString ext = getExtension(url.fileName());

	static const QStringList ignored { "php", "html" };
	if (ext.isEmpty() || ignored.contains(ext)) {
		const QString fullUrl = url.toString();
		const int lastSlash = fullUrl.lastIndexOf('/');
		return getExtension(fullUrl.mid(lastSlash + 1));
	}

	return ext;
}

QUrl setExtension(QUrl url, const QString &extension)
{
	QString path = url.path();

	const int lastSlash = path.lastIndexOf('/');
	const int lastDot = path.mid(lastSlash + 1).lastIndexOf('.');
	if (lastDot != -1) {
		const int doubleDot = path.mid(lastDot + 1).indexOf(':');
		url.setPath(path.left(lastDot + lastSlash + 1) + "." + extension + (doubleDot != -1 ? path.mid(lastDot + doubleDot + 1) : ""));
	}

	return url;
}

bool isUrl(const QString &str)
{
	static const QRegularExpression regexUrl(QStringLiteral("^https?://[^\\s/$.?#].[^\\s]*$"));
	return regexUrl.match(str).hasMatch();
}

QString fixFilename(QString filename, QString path, int maxLength, bool invalidChars)
{
	const QString sep = QDir::separator();
	filename = QDir::toNativeSeparators(filename);
	path = QDir::toNativeSeparators(path);
	if (!path.endsWith(sep) && !path.isEmpty() && !filename.isEmpty()) {
		path += sep;
	}

	#ifdef Q_OS_WIN
		return fixFilenameWindows(filename, path, maxLength, invalidChars);
	#else
		return fixFilenameLinux(filename, path, maxLength, invalidChars);
	#endif
}

// https://stackoverflow.com/questions/26629382/how-to-shorten-qstring-in-a-way-that-when-converted-to-utf-8-it-is-shorter-than
bool cutStringToUtf8Bytes(QString &str, int limit)
{
	QByteArray output = str.toUtf8();
	if (output.size() > limit) {
		int truncateAt = 0;
		for (int i = limit; i > 0; i--) {
			if ((output[i] & 0xC0) != 0x80) {
				truncateAt = i;
				break;
			}
		}
		output.truncate(truncateAt);
		str = QString::fromUtf8(output);
		return true;
	}
	return false;
}

QString fixFilenameLinux(const QString &fn, const QString &path, int maxLength, bool invalidChars)
{
	Q_UNUSED(invalidChars);

	// Fix parameters
	const QString sep = QStringLiteral("/");
	maxLength = maxLength == 0 ? 255 : maxLength;
	QString filename = path + fn;

	// Divide filename
	QStringList parts = filename.split(sep);
	QString file, ext;
	if (!fn.isEmpty()) {
		file = parts.takeLast();;
		const int lastDot = file.lastIndexOf('.');
		if (lastDot != -1) {
			ext = file.right(file.length() - lastDot - 1);
			file = file.left(lastDot);
		}
	}

	// Fix directories (each part cannot be more than 255 bytes)
	for (QString &part : parts) {
		cutStringToUtf8Bytes(part, 255);
	}

	// A filename cannot exceed 255 bytes
	const int extlen = ext.isEmpty() ? 0 : ext.length() + 1;
	cutStringToUtf8Bytes(file, maxLength - extlen);

	// Join parts back
	QString dirpart = parts.join(sep);
	filename = (dirpart.isEmpty() ? QString() : dirpart + (!fn.isEmpty() ? sep : QString())) + file;

	// Get separation between filename and path
	int index = -1;
	const int pathGroups = path.count(sep);
	for (int i = 0; i < pathGroups; ++i) {
		index = filename.indexOf(sep, index + 1);
	}

	// Put extension and drive back
	filename = filename + (!ext.isEmpty() ? "." + ext : QString());
	if (!fn.isEmpty()) {
		filename = filename.right(filename.length() - index - 1);
	}

	QFileInfo fi(filename);
	QString suffix = fi.suffix();
	filename = (fi.path() != "." ? fi.path() + "/" : QString()) + fi.completeBaseName().left(245) + (suffix.isEmpty() ? QString() : "." + fi.suffix());

	return filename;
}

#ifndef MAX_PATH
	#define MAX_PATH 260
#endif

QString fixFilenameWindows(const QString &fn, const QString &path, int maxLength, bool invalidChars)
{
	// Fix parameters
	const QString sep = QStringLiteral("\\");
	maxLength = maxLength == 0 ? MAX_PATH : maxLength;
	QString filename = (path + fn).trimmed();

	// Don't do anything when given the "\\?\" prefix (https://stackoverflow.com/questions/21194530/what-does-mean-when-prepended-to-a-file-path)
	if (filename.startsWith(R"(\\?\)")) {
		return fn;
	}

	// Drive
	QString drive;
	if (filename.mid(1, 2) == QLatin1String(":\\")) {
		drive = filename.left(3);
		filename = filename.right(filename.length() - 3);
	}

	// Forbidden characters
	if (invalidChars) {
		filename.replace('<', '_').replace('>', '_').replace(':', '_').remove('"').replace('/', '_').replace('|', '_').remove('?').replace('*', '_');
	}

	// Fobidden directories or filenames
	static const QStringList forbidden { "CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9" };

	// Divide filename
	QStringList parts = filename.split(sep);
	QString file, ext;
	if (!fn.isEmpty()) {
		file = parts.takeLast();
		const int lastDot = file.lastIndexOf('.');
		if (lastDot != -1) {
			ext = file.right(file.length() - lastDot - 1);
			file = file.left(lastDot);
		}
	}

	// Fix directories
	for (QString &part : parts) {
		// A part cannot be one in the forbidden list
		if (invalidChars && forbidden.contains(part, Qt::CaseInsensitive)) {
			part = part + "!";
		}

		// A part cannot finish by a period
		while (invalidChars && part.endsWith('.')) {
			part = part.left(part.length() - 1).trimmed();
		}

		// A part cannot start or finish with a space
		part = part.trimmed();

		// A part should still allow creating a file
		if (part.length() > maxLength - 12) {
			part = part.left(qMax(0, maxLength - 12)).trimmed();
		}
	}

	// Join parts back
	QString dirpart = parts.join(sep);
	if (dirpart.length() > maxLength - 12) {
		dirpart = dirpart.left(qMax(0, maxLength - 12)).trimmed();
	}
	filename = (dirpart.isEmpty() ? QString() : dirpart + (!fn.isEmpty() ? sep : QString())) + file;

	// A filename cannot exceed MAX_PATH (-1 for <NUL> and -3 for drive "C:\")
	if (filename.length() > maxLength - 1 - 3 - ext.length() - 1) {
		filename = filename.left(qMax(0, maxLength - 1 - 3 - ext.length() - 1)).trimmed();
	}

	// Get separation between filename and path
	int index = -1;
	const int pathGroups = path.count(sep);
	for (int i = 0; i < pathGroups - (!drive.isEmpty() ? 1 : 0); ++i) {
		index = filename.indexOf(sep, index + 1);
	}
	index += drive.length();

	// Put extension and drive back
	filename = drive + filename + (!ext.isEmpty() ? "." + ext : QString());
	if (!fn.isEmpty()) {
		filename = filename.right(filename.length() - index - 1);
	}

	return filename;
}


QString getExtensionFromHeader(const QByteArray &data12)
{
	const QByteArray data8 = data12.left(8);
	const QByteArray data48 = data12.mid(4, 8);
	const QByteArray data6 = data12.left(6);
	const QByteArray data4 = data12.left(4);
	const QByteArray data3 = data12.left(3);
	const QByteArray data2 = data12.left(2);

	// GIF
	if (data6 == "GIF87a" || data6 == "GIF89a") {
		return QStringLiteral("gif");
	}

	// PNG
	if (data8 == "\211PNG\r\n\032\n") {
		return QStringLiteral("png");
	}

	// JPG
	if (data3 == "\377\330\377") {
		return QStringLiteral("jpg");
	}

	// BMP
	if (data2 == "BM") {
		return QStringLiteral("bmp");
	}

	// WEBM
	if (data4 == "\032\105\337\243") {
		return QStringLiteral("webm");
	}

	// MP4
	if (data48 == "ftyp3gp5" || data48 == "ftypMSNV" || data48 == "ftypisom") {
		return QStringLiteral("mp4");
	}

	// SWF
	if (data3 == "FWS" || data3 == "CWS" || data3 == "ZWS") {
		return QStringLiteral("swf");
	}

	// FLV
	if (data4 == "FLV\001") {
		return QStringLiteral("flv");
	}

	// ICO
	if (data4 == QByteArray("\000\000\001\000", 4)) {
		return QStringLiteral("ico");
	}

	return QString();
}


bool testModeEnabled = false;
void setTestModeEnabled(bool testMode)
{
	testModeEnabled = testMode;
}
bool isTestModeEnabled()
{
	return testModeEnabled;
}


QString fixCloudflareEmail(const QString &a)
{
	QString s;
	int r = a.mid(0, 2).toInt(nullptr, 16);
	for (int j = 2; j < a.length(); j += 2) {
		int c = a.mid(j, 2).toInt(nullptr, 16) ^ r;
		s += QString(QChar(c));
	}
	return s;
}
QString fixCloudflareEmails(QString html)
{
	static const QRegularExpression rx("<span class=\"__cf_email__\" data-cfemail=\"([^\"]+)\">\\[[^<]+\\]<\\/span>");
	auto matches = rx.globalMatch(html);
	while (matches.hasNext()) {
		auto match = matches.next();
		const QString email = fixCloudflareEmail(match.captured(1));
		html.replace(match.captured(0), email);
	}
	return html;
}


QString getFileMd5(const QString &path)
{
	QFile file(path);
	if (!file.open(QFile::ReadOnly)) {
		return QString();
	}
	return QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
}

QString getFilenameToken(const QString &fileName, const QString &format, const QString &token, const QString &regex = ".+")
{
	QString reg = "^" + QRegularExpression::escape(format) + "$";
	#ifdef Q_OS_WIN
		reg.replace("\\\\", "[\\\\/]");
	#endif

	static const QString escapedPercent = QRegularExpression::escape("%");
	static const QRegularExpression regx(escapedPercent + "([^%]*)" + escapedPercent);
	auto matches = regx.globalMatch(format);
	while (matches.hasNext()) {
		const auto match = matches.next();
		const auto cap = QRegularExpression::escape(match.captured(0));
		const bool isToken = match.captured(1) == token;
		reg.replace(cap, isToken ? QString("(?<token>%1)").arg(regex) : QStringLiteral("(.+?)"));
	}

	const QRegularExpression rx(reg, QRegularExpression::CaseInsensitiveOption);
	const auto match = rx.match(fileName);
	if (match.hasMatch()) {
		return match.captured("token");
	} else {
		log(QStringLiteral("Unable to detect %1 file `%2`").arg(token, fileName), Logger::Warning);
	}

	return QString();
}
QString getFilenameMd5(const QString &fileName, const QString &format)
{
	return getFilenameToken(fileName, format, "md5", "[0-9A-F]{32,}");
}
QString getFilenameId(const QString &fileName, const QString &format)
{
	return getFilenameToken(fileName, format, "id", "[0-9]+");
}


QString parseMarkdown(QString str)
{
	// Windows EOL
	str.replace("\\r\\n", "\\n");

	// Headers
	static const QRegularExpression header(QStringLiteral("^(#+)([^#].*)$"), QRegularExpression::MultilineOption);
	auto matches = header.globalMatch(str);
	while (matches.hasNext()) {
		auto match = matches.next();
		const int level = qMax(1, qMin(6, match.captured(1).length()));
		const QString result = "<h" + QString::number(level) + ">" + match.captured(2).trimmed() + "</h" + QString::number(level) + ">";
		str.replace(match.captured(0), result);
	}

	str = parseGithubLinks(str);

	// Line breaks to HTML
	str.replace("\n", "<br/>");

	return str;
}

QString parseGithubLinks(QString str)
{
	// Issue links
	static const QRegularExpression issueLinks("(issue|fix) #(\\d+)");
	str.replace(issueLinks, "\\1 <a href='" + QStringLiteral(PROJECT_GITHUB_URL) + R"(/issues/\2'>#\2</a>)");

	// Commit hashes
	static const QRegularExpression commitHashes("([0-9a-f]{8})([0-9a-f]{32})");
	str.replace(commitHashes, "<a href='" + QStringLiteral(PROJECT_GITHUB_URL) + R"(/commit/\1\2'>\1</a>)");

	return str;
}


/**
 * Converts a QFont to a CSS string.
 * @param	font	The font to convert.
 * @return	The CSS font.
 */
QString qFontToCss(const QFont &font)
{
	QString style;
	switch (font.style())
	{
		case QFont::StyleNormal: style = "normal"; break;
		case QFont::StyleItalic: style = "italic"; break;
		case QFont::StyleOblique: style = "oblique"; break;
	}

	QString size;
	if (font.pixelSize() == -1) {
		size = QString::number(font.pointSize()) + "pt";
	} else {
		size = QString::number(font.pixelSize()) + "px";
	}

	// Should be "font.weight() + 100", but linux doesn't handle weight the same way windows do
	const QString weight = QString::number(font.weight());

	QStringList decorations;
	if (font.strikeOut()) {
		decorations.append("line-through");
	}
	if (font.underline()) {
		decorations.append("underline");
	}

	return "font-family:'" + font.family() + "'; font-size:" + size + "; font-style:" + style + "; font-weight:" + weight + "; text-decoration:" + (decorations.isEmpty() ? "none" : decorations.join(" ")) + ";";
}

QFont qFontFromString(const QString &str)
{
	QFont font;
	font.fromString(str);
	if (font.family().isEmpty()) {
		font.setFamily(font.defaultFamily());
	}
	return font;
}

bool isFileParentWithSuffix(const QString &fileName, const QString &parent, const QStringList &suffixes)
{
	for (const QString &suffix : suffixes) {
		if (fileName == parent + suffix) {
			return true;
		}
	}
	return false;
}
QList<QPair<QString, QStringList>> listFilesFromDirectory(const QDir &dir, const QStringList &suffixes)
{
	auto files = QList<QPair<QString, QStringList>>();

	QDirIterator it(dir, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		it.next();

		if (it.fileInfo().isDir()) {
			continue;
		}

		QString path = it.filePath();
		const QString fileName = path.right(path.length() - dir.absolutePath().length() - 1);

		if (!files.isEmpty()) {
			const QString &previous = files.last().first;
			if (isFileParentWithSuffix(fileName, previous, suffixes)) {
				files.last().second.append(fileName);
				continue;
			}
		}

		files.append(QPair<QString, QStringList>(fileName, QStringList()));
	}

	return files;
}

QUrl removeCacheBuster(QUrl url)
{
	const QString query = url.query();
	if (query.isEmpty()) {
		return url;
	}

	// Only remove ?integer
	bool ok;
	query.toInt(&ok);
	if (ok) {
		url.setQuery(QString());
	}

	return url;
}

bool isVariantEmpty(const QVariant &value)
{
	switch (value.type())
	{
		case QVariant::Type::Int: return value.toInt() == 0;
		case QVariant::Type::List: return value.toList().isEmpty();
		case QVariant::Type::Map: return value.toMap().isEmpty();
		case QVariant::Type::String: return value.toString().isEmpty();
		case QVariant::Type::StringList: return value.toStringList().isEmpty();
		default: return false;
	}
}

QString decodeHtmlEntities(const QString &html)
{
	QByteArray data = html.toUtf8();
	const char *src = data.constData();
	auto *dest = new char[strlen(src) + 1];
	decode_html_entities_utf8(dest, src);
	QString ret = QString::fromUtf8(dest);
	delete[] dest;
	return ret;
}

bool canCreateLinkType(const QString &type, const QString &dir)
{
	const QString basePath = dir + QDir::separator() + "link_test";
	const QString linkFrom = basePath + "_from";
	const QString linkTo = basePath + "_to";

	// Create empty "source" file
	QFile f(linkFrom);
	f.open(QFile::WriteOnly | QFile::Truncate);
	f.close();

	const bool ok = createLink(linkFrom, linkTo, type);

	// Clean-up
	QFile::remove(linkFrom);
	QFile::remove(linkTo);

	return ok;
}

bool createLink(const QString &from, const QString &to, const QString &type)
{
	#ifdef Q_OS_WIN
		if (type == "link") {
			return QFile::link(from, to + ".lnk");
		}/* else if (type == "symlink") {
	        wchar_t *wFrom = toWCharT(from);
	        wchar_t *wTo = toWCharT(to);
	        const bool res = CreateSymbolicLinkW(wTo, wFrom, 0x2);
	        delete[] wFrom;
	        delete[] wTo;
	        if (!res) {
	        	log(QStringLiteral("Unable to create symbolic link from `%1` to `%2`: %3 - %4").arg(from, to).arg(lastError()).arg(lastErrorString()), Logger::Error);
	        }
	        return res;
	    }*/
		else if (type == "hardlink") {
			wchar_t *wFrom = toWCharT(from);
			wchar_t *wTo = toWCharT(to);
			const bool res = CreateHardLinkW(wTo, wFrom, NULL);
			delete[] wFrom;
			delete[] wTo;
			if (!res) {
				log(QStringLiteral("Unable to create hard link from `%1` to `%2`: %3 - %4").arg(from, to).arg(lastError()).arg(lastErrorString()), Logger::Error);
			}
			return res;
		}
	#else
		if (type == "link") {
			return QFile::link(from, to);
		} else if (type == "hardlink") {
			const int res = link(from.toStdString().c_str(), to.toStdString().c_str());
			if (res < 0) {
				log(QStringLiteral("Unable to create hard link from `%1` to `%2` (%3 - %4)").arg(from, to).arg(lastError()).arg(lastErrorString()), Logger::Error);
				return false;
			}
			return true;
		}
	#endif
	log(QStringLiteral("Invalid link type '%1'").arg(type), Logger::Error);
	return false;
}


QKeySequence getKeySequence(QSettings *settings, const QString &key, QKeySequence::StandardKey standardDefault, const QKeySequence &altDefault)
{
	const auto standards = QKeySequence::keyBindings(standardDefault);
	if (standards.isEmpty()) {
		return getKeySequence(settings, key, altDefault);
	}
	return getKeySequence(settings, key, standards.first());
}

QKeySequence getKeySequence(QSettings *settings, const QString &key, const QKeySequence &def)
{
	QString val = settings->value(key).toString();
	if (val.isEmpty()) {
		return def;
	}
	return QKeySequence(val);
}


QString rectToString(const QRect &rect)
{
	if (rect.isNull()) {
		return "";
	}
	return QStringLiteral("%1;%2;%3;%4")
		.arg(rect.x())
		.arg(rect.y())
		.arg(rect.width())
		.arg(rect.height());
}

QRect stringToRect(const QString &str)
{
	const QStringList parts = str.split(';');
	if (parts.count() == 4) {
		return QRect(parts[0].toInt(), parts[1].toInt(), parts[2].toInt(), parts[3].toInt());
	}
	return {};
}


void renameSettingsGroup(QSettings *settings, const QString &before, const QString &after)
{
	settings->beginGroup(before);
	const QStringList keys = settings->allKeys();
	settings->endGroup();

	for (const QString &key : keys) {
		renameSettingsKey(settings, before + "/" + key, after + "/" + key);
	}
}

void renameSettingsKey(QSettings *settings, const QString &before, const QString &after)
{
	settings->setValue(after, settings->value(before));
	settings->remove(before);
}


/**
 * Helper function to know if a byte array contains HTML.
 * Only performs a basic check and might fail for XML or some differently formatted documents.
 */
bool isHtml(const QByteArray &data)
{
	const QString left = QString(data.left(100)).trimmed();
	return left.startsWith("<!DOCTYPE", Qt::CaseInsensitive)
		|| left.startsWith("<html>", Qt::CaseInsensitive);
}
