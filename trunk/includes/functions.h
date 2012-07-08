#ifndef HEADER_FUCTIONS
#define HEADER_FUCTIONS

#include <QMap>
#include <QStringList>
#include <QDateTime>
#include <QFont>
#include <QDomElement>
#include <QLayout>



//typedef QMap<QString,QString> QStringMap;
//typedef QMap<QString,QMap<QString,QString> > QStringMapMap;



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
float		round(float n, unsigned d);
void		clearLayout(QLayout *layout);
QString		stripTags(QString);

void		showInGraphicalShell(const QString &);
void		shutDown(int timeout = 0);
void		openTray();

void		log(QString, Log type = Info);
void		logUpdate(QString);

QMap<QString,QString>		loadFavorites();
QStringList					loadViewItLater();
QStringList					loadIgnored();

QString						mapToString(QMap<QString,QString> map, QString gen = "{gen}", QString mid = "{mid}");
QMap<QString,QString>		domToMap(QDomElement);
QMap<QString,QString>		stringToMap(QString map, QString gen = "{gen}", QString mid = "{mid}");

QMap<QString,QStringList>	getCustoms();
QMap<QString,QString>		getFilenames();


#endif
