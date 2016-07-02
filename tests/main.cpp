#include <QTest>
#include "mainwindow.h"
#include "models/test-tag.h"

QMap<QDateTime, QString> _log;
QMap<QString, QString> _md5;
mainWindow *_mainwindow;


int main(int argc, char** argv)
{
   int status = 0;
   {
	  TestTag tc;
	  status |= QTest::qExec(&tc, argc, argv);
   }
   return status;
}
