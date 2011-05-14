#ifndef HEADER_FUCTIONS
#define HEADER_FUCTIONS

#include <QtGui>

void error(QWidget *, QString);
void sortNonCaseSensitive(QStringList &);
QDateTime qDateTimeFromString(QString);
QMap<QString,QString> loadFavorites();

#endif
