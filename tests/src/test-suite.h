#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <QList>
#include <QObject>
#include <QtTest>
#include "models/profile.h"


#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4005)
#else
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmacro-redefined"
#endif

#define QTEST_MAIN(CLASS_NAME) static CLASS_NAME instance;

#ifdef _MSC_VER
	#pragma warning(pop)
#else
	#pragma GCC diagnostic pop
#endif


#ifdef _MSC_VER
	#include <crtdbg.h>

	struct MemoryLeakChecker
	{
		_CrtMemState state_initial;
		_CrtMemState state_end;
		_CrtMemState diff;

		MemoryLeakChecker()
		{
			_CrtMemCheckpoint(&state_initial);
		}

		~MemoryLeakChecker()
		{
			_CrtMemCheckpoint(&state_end);

			// Ensure there are no leaks
			bool memoryLeaks = _CrtMemDifference(&diff, &state_initial, &state_end);
			QCOMPARE(memoryLeaks, false);

			// Dump leaked blocks
			if (memoryLeaks)
				_CrtMemDumpStatistics(&diff);
		}
	};
#else
	struct MemoryLeakChecker
	{ };
#endif


class TestSuite : public QObject
{
	Q_OBJECT

	public:
		TestSuite();
		void setupSource(const QString &site, QString dir = QString());
		void setupSite(const QString &site, const QString &source, QString dir = QString());
		static QList<TestSuite*> &getSuites();
};

#endif // TEST_SUITE_H
