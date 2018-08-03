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

	// A possible format to filter by test suite it to pass their names as arguments
	QStringList testSuites;
	for (int i = 1; i < argc; ++i)
	{
		QString arg(argv[i]);
		if (!arg.startsWith('-') && !arg.startsWith("test"))
			testSuites.append(argv[i]);
	}

	// Used for networking and finding test resource files
	setTestModeEnabled(true);

	// Run all selected test suites
	int errorCode = 0;
	for (TestSuite *suite : TestSuite::getSuites())
	{
		if (!testSuites.isEmpty() && !testSuites.contains(suite->metaObject()->className()))
			continue;

		errorCode |= QTest::qExec(suite, argc, argv);
		std::cout << std::endl;
	}

	return errorCode;
}
