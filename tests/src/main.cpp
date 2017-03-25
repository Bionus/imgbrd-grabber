#include <QTest>
#include "test-suite.h"
#include "custom-network-access-manager.h"
#include <iostream>


int main(int argc, char *argv[])
{
	#ifdef HEADLESS
		QCoreApplication a(argc, argv);
	#else
		QGuiApplication a(argc, argv);
	#endif

	QStringList testSuites;
	for (int i = 1; i < argc; ++i)
		testSuites.append(argv[i]);

	QMap<QString,int> results;
	int failed = 0;

	CustomNetworkAccessManager::TestMode = true;

	for (QObject *suite : TestSuite::suites)
	{
		if (!testSuites.isEmpty() && !testSuites.contains(suite->metaObject()->className()))
			continue;

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
