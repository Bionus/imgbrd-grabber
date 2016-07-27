#include <QTest>
#include "mainwindow.h"
#include "test-suite.h"

QMap<QDateTime, QString> _log;
QMap<QString, QString> _md5;
mainWindow *_mainwindow;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int failed = 0;
    for (QObject *suite : TestSuite::suites)
    {
        int result = QTest::qExec(suite);
        if (result != 0)
        {
            failed++;
        }
    }

    return failed;
}
