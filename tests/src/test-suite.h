#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <QtTest>
#include <QObject>
#include <QList>

#include "models/profile.h"


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
		explicit TestSuite();
		void setupSource(QString site);
		void setupSite(QString site, QString source);
		static QList<QObject*> &getSuites();

	protected:
		Profile profile;
};

#endif // TEST_SUITE_H
