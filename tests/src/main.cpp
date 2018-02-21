#include <QTest>
#include <iostream>
#include "custom-network-access-manager.h"
#include "functions.h"
#include "test-suite.h"


int main(int argc, char *argv[])
{
	#ifdef HEADLESS
		QCoreApplication a(argc, argv);
	#else
		QGuiApplication a(argc, argv);
	#endif

	QStringList testSuites;
	testSuites.reserve(argc - 1);
	for (int i = 1; i < argc; ++i)
		testSuites.append(argv[i]);

	QMap<QString, int> results;
	int failed = 0;

	setTestModeEnabled(true);

	for (TestSuite *suite : TestSuite::getSuites())
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

	for (auto it = results.begin(); it != results.end(); ++it)
	{
		std::cout << '[' << (it.value() != 0 ? "FAIL" : "OK") << "] " << it.key().toStdString() << std::endl;
	}

	return failed;
}
