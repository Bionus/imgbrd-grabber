#include <QTest>
#include "test-suite.h"
#include "custom-network-access-manager.h"
#include "functions.h"
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

	setTestModeEnabled(true);

	for (QObject *suite : TestSuite::getSuites())
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

	for (const QString &key : results.keys())
	{
		std::cout << '[' << (results.value(key) != 0 ? "FAIL" : "OK") << "] " << key.toStdString() << std::endl;
	}

	return failed;
}
