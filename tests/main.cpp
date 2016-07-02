#include <QTest>
#include "mainwindow.h"

#include "models/tag-test.h"
#include "models/favorite-test.h"

QMap<QDateTime, QString> _log;
QMap<QString, QString> _md5;
mainWindow *_mainwindow;


int main(int argc, char** argv)
{
   int status = 0;
   {
	  TagTest tc;
	  status |= QTest::qExec(&tc, argc, argv);
   }
   {
	  FavoriteTest tc;
	  status |= QTest::qExec(&tc, argc, argv);
   }
   return status;
}
