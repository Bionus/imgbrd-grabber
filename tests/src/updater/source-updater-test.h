#ifndef SOURCE_UPDATER_TEST_H
#define SOURCE_UPDATER_TEST_H

#include "test-suite.h"


class SourceUpdaterTest : public TestSuite
{
	Q_OBJECT

	private slots:
#if defined(Q_OS_WIN) && 0
		void testNoUpdate();
		void testChanged();
#endif
};

#endif // SOURCE_UPDATER_TEST_H
