#ifndef HEADER_FUCTIONS
#define HEADER_FUCTIONS

#include <QMap>
#include <QStringList>
#include <QDateTime>
#include <QFont>
#include <QDomElement>
#include <QLayout>



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
QString		getUnit(int*);
QString		getUnit(float*);

void		showInGraphicalShell(const QString &);
void		shutDown(int timeout = 0);
void		openTray();

void		log(QString, Log type = Info);
void		logUpdate(QString);

void		loadMd5s();
void		saveMd5s();
QString		md5Exists(QString);
void		addMd5(QString, QString);
void		removeMd5(QString);

QMap<QString,QString>		loadFavorites();
QStringList					loadViewItLater();
QStringList					loadIgnored();

QString						mapToString(QMap<QString,QString> map, QString gen = "{gen}", QString mid = "{mid}");
QMap<QString,QString>		domToMap(QDomElement);
QMap<QString,QString>		stringToMap(QString map, QString gen = "{gen}", QString mid = "{mid}");

QMap<QString,QStringList>	getCustoms();
QMap<QString,QString>		getFilenames();


#endif
