#ifndef HEADER_FUCTIONS
#define HEADER_FUCTIONS

#include <QtGui>
#include <QtXml>



typedef QMap<QString,QString> QStringMap;
typedef QMap<QString,QStringMap> QStringMapMap;



enum Log
{
	Info,
	Notice,
	Warning,
	Error
};



void		error(QWidget *, QString);
void		sortNonCaseSensitive(QStringList &);
QDateTime	qDateTimeFromString(QString s, int d = 5);
QString		savePath(QString s = "");
int			levenshtein(QString, QString);
QString		validateFilename(QString);
void		showInGraphicalShell(const QString &);
QString		qfonttocss(QFont);
QStringMap	domToMap(QDomElement);

QMap<QString,QStringList>	getCustoms();
QMap<QString,QString>		getFilenames();

QStringMap	loadFavorites();
QStringList	loadViewItLater();
QStringList	loadIgnored();

void		log(QString, Log type = Info);
void		logUpdate(QString);

#endif
