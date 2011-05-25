#ifndef HEADER_FUCTIONS
#define HEADER_FUCTIONS

#include <QtGui>

void error(QWidget *, QString);
void sortNonCaseSensitive(QStringList &);
QDateTime qDateTimeFromString(QString s, int d = 5);
QMap<QString,QString> loadFavorites();
QString savePath(QString s = "");

#endif
