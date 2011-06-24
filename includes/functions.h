#ifndef HEADER_FUCTIONS
#define HEADER_FUCTIONS

#include <QtGui>
#include <QtXml>

void error(QWidget *, QString);
void sortNonCaseSensitive(QStringList &);
QDateTime qDateTimeFromString(QString s, int d = 5);
QMap<QString,QString> loadFavorites();
QString savePath(QString s = "");
QString validateFilename(QString);
void showInGraphicalShell(const QString &);
QString qfonttocss(QFont);
QMap<QString,QString> domToMap(QDomElement);

#endif
