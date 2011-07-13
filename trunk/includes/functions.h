#ifndef HEADER_FUCTIONS
#define HEADER_FUCTIONS

#include <QtGui>
#include <QtXml>



typedef QMap<QString,QString> QStringMap;
typedef QMap<QString,QStringMap> QStringMapMap;



void		error(QWidget *, QString);
void		sortNonCaseSensitive(QStringList &);
QDateTime	qDateTimeFromString(QString s, int d = 5);
QString		savePath(QString s = "");
QString		validateFilename(QString);
void		showInGraphicalShell(const QString &);
QString		qfonttocss(QFont);
QStringMap	domToMap(QDomElement);

QStringMap	loadFavorites();
QStringList	loadViewItLater();

void		log(QString);
void		logUpdate(QString);

#endif
