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
QString		qfonttocss(QFont);
QStringMap	domToMap(QDomElement);
float		round(float n, unsigned d);

void		showInGraphicalShell(const QString &);
void		shutDown(int timeout = 0);
void		openTray();

QStringMap	loadFavorites();
QStringList	loadViewItLater();
QStringList	loadIgnored();

void		log(QString, Log type = Info);
void		logUpdate(QString);

QMap<QString,QStringList>	getCustoms();
QMap<QString,QString>		getFilenames();


#endif
