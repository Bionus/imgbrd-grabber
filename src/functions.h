#ifndef HEADER_FUNCTIONS
#define HEADER_FUNCTIONS

#include <QMap>
#include <QStringList>
#include <QDateTime>
#include <QFont>
#include <QDomElement>
#include <QLayout>
#include <algorithm>
#include <QString>
#include "models/favorite.h"

#define DONE()	logUpdate(QObject::tr(" Fait"))



enum Log
{
	Info,
	Notice,
	Warning,
	Error
};


void		error(QWidget *, QString);
void		sortNonCaseSensitive(QStringList &);
QDateTime	qDateTimeFromString(QString s);
QString		savePath(QString s = "", bool exists = false);
int			levenshtein(QString, QString);
QString		validateFilename(QString);
QString		qfonttocss(QFont);
float		round(float n, int d = 0);
void		clearLayout(QLayout *layout);
QString		stripTags(QString);
QString		getUnit(int*);
QString		getUnit(float*);
QString		getExtension(QString url);
QString		setExtension(QString url, QString extension);

bool		setFileCreationDate(QString path, QDateTime time);
void		showInGraphicalShell(const QString &);
void		shutDown(int timeout = 0);
void		openTray();

void		setLogFile(QString path);
void		log(QString, Log type = Info);
void		logCommand(QString);
void		logCommandSql(QString);
void		logUpdate(QString);

void		loadMd5s();
void		saveMd5s();
void		saveMd5(QString md5, QString path);
QString		md5Exists(QString);
void		addMd5(QString, QString);
void		setMd5(QString, QString);
void		removeMd5(QString);

QString fixFilename(QString filename, QString path = "", int maxlength = 0, bool invalidChars = true);
QString fixFilenameWindows(QString filename, QString path = "", int maxlength = 0, bool invalidChars = true);
QString fixFilenameLinux(QString filename, QString path = "", int maxlength = 0, bool invalidChars = true);

QList<Favorite> loadFavorites();
QStringList		loadViewItLater();
QStringList		loadIgnored();

QString						mapToString(QMap<QString,QString> map, QString gen = "{gen}", QString mid = "{mid}");
QMap<QString,QString>		domToMap(QDomElement);
QMap<QString,QString>		stringToMap(QString map, QString gen = "{gen}", QString mid = "{mid}");

QMap<QString,QStringList>	getCustoms();
QMap<QString,QPair<QString,QString>>	getFilenames();



template <typename T>
QList<T> reversed(const QList<T> & in)
{
	QList<T> result;
	std::reverse_copy(in.begin(), in.end(), std::back_inserter(result));
	return result;
}
inline bool sortByNote(const Favorite &s1, const Favorite &s2)
{ return s1.getNote() < s2.getNote(); }
inline bool sortByName(const Favorite &s1, const Favorite &s2)
{ return s1.getName().toLower() < s2.getName().toLower(); }
inline bool sortByLastviewed(const Favorite &s1, const Favorite &s2)
{ return s1.getLastViewed() < s2.getLastViewed(); }


#endif
