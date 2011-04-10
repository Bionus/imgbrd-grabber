#include <QApplication>
#include <QtGui>
#include "functions.h"

void error(QWidget *p, QString d)
{ QMessageBox::critical(p, "Erreur", d); }

void sortNonCaseSensitive(QStringList &sList)
{
	QMap<QString, QString> strMap;
	foreach (QString str, sList)
	{ strMap.insert( str.toLower(), str); }
	sList = strMap.values();
}

QDateTime qDateTimeFromString(QString str)
{
	QDateTime date;
	QList<QString> months;
	months << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" << "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";
	QTime time(str.mid(11,2).toInt(), str.mid(14,2).toInt(), str.mid(17,2).toInt());
	date.setDate(QDate(str.mid(26,4).toInt(), months.indexOf(str.mid(4,3))+1, str.mid(8,2).toInt()+(str.mid(11,2).toInt() >= 18)));
	date.setTime(time.addSecs(60*60*5)); // We turn the date back to GMT
	date.setTimeSpec(Qt::UTC);
	date = date.toLocalTime(); // Then to user's time
	return date;
}
