#include <QTest>
#include "mainwindow.h"
#include "test-suite.h"
#include "custom-network-access-manager.h"
#include <iostream>

QMap<QDateTime, QString> _log;
mainWindow *_mainwindow;


int main(int argc, char *argv[])
{
	#ifdef HEADLESS
		QCoreApplication a(argc, argv);
	#else
		QGuiApplication a(argc, argv);
	#endif

	QMap<QString,int> results;
	int failed = 0;

	CustomNetworkAccessManager::TestMode = true;

	for (QObject *suite : TestSuite::suites)
	{
		int result = QTest::qExec(suite);
		results.insert(suite->metaObject()->className(), result);
		if (result != 0)
		{
			failed++;
		}
	}

	for (auto key : results.keys())
	{
		std::cout << '[' << (results.value(key) != 0 ? "FAIL" : "OK") << "] " << key.toStdString() << std::endl;
	}

	return failed;
}
