#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QMap>
#include <QString>
#include "backports/backports.h"


class ConditionalFilename;
class QDateTime;
class QDir;
class QFont;
class QJSValue;
class QStringList;
class QSettings;
class QUrl;
class QVariant;


// Filesize units
#if defined(Q_OS_WIN)
	// 1 KB = 1024 B
	#define FILESIZE_UNITS QStringList { QObject::tr("B"), QObject::tr("KB"), QObject::tr("MB"), QObject::tr("GB") };
	#define FILESIZE_MULTIPLIER 1024
#elif defined(Q_OS_MACOS)
	// 1 KB = 1000 B
	#define FILESIZE_UNITS QStringList { QObject::tr("B"), QObject::tr("KB"), QObject::tr("MB"), QObject::tr("GB") };
	#define FILESIZE_MULTIPLIER 1000
#else
	// 1 KiB = 1024 B
	#define FILESIZE_UNITS QStringList { QObject::tr("B"), QObject::tr("KiB"), QObject::tr("MiB"), QObject::tr("GiB") };
	#define FILESIZE_MULTIPLIER 1024
#endif


QDateTime qDateTimeFromString(const QString &str);
QString savePath(const QString &file = "", bool exists = false, bool writable = true);
bool copyRecursively(QString srcFilePath, QString tgtFilePath);
int levenshtein(QString, QString);
QString stripTags(QString);
QString getUnit(double *size);
QString formatFilesize(double size);
QString getExtension(const QUrl &url);
QUrl setExtension(QUrl url, const QString &extension);
bool isUrl(const QString &str);
bool isVariantEmpty(const QVariant &value);

bool setFileCreationDate(const QString &path, const QDateTime &datetime);
void shutDown(int timeout = 0);
void openTray();

QString fixFilename(QString filename, QString path = "", int maxLength = 0, bool invalidChars = true);
QString fixFilenameWindows(const QString &fn, const QString &path = "", int maxLength = 0, bool invalidChars = true);
QString fixFilenameLinux(const QString &fn, const QString &path = "", int maxLength = 0, bool invalidChars = true);

QMap<QString, QStringList> getCustoms(QSettings *settings);
QList<ConditionalFilename> getFilenames(QSettings *settings);
QMap<int, QMap<QString, QVariant>> getExternalLogFiles(QSettings *settings);
QStringList getExternalLogFilesSuffixes(QSettings *settings);

QStringList removeWildards(const QStringList &elements, const QStringList &remove);
QString getExtensionFromHeader(const QByteArray &data12);

void setTestModeEnabled(bool testMode);
bool isTestModeEnabled();

QString fixCloudflareEmail(const QString &a);
QString fixCloudflareEmails(QString html);

QString getFileMd5(const QString &path);
QString getFilenameMd5(const QString &fileName, const QString &format);

QString parseMarkdown(QString str);
QString decodeHtmlEntities(const QString &html);

QString qFontToCss(const QFont &font);
QFont qFontFromString(const QString &str);

QList<QPair<QString, QStringList>> listFilesFromDirectory(const QDir &dir, const QStringList &suffixes);

QUrl removeCacheBuster(QUrl url);
QStringList jsToStringList(const QJSValue &val);



template <typename T>
QList<T> reversed(const QList<T> &in)
{
	QList<T> result;
	std::reverse_copy(in.begin(), in.end(), std::back_inserter(result));
	return result;
}

#endif // FUNCTIONS_H
