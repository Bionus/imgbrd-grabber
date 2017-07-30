#ifndef SOURCE_UPDATER_TEST_H
#define SOURCE_UPDATER_TEST_H

#include "test-suite.h"


class SourceUpdaterTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testNoUpdate();
#ifdef Q_OS_WIN
		void testChanged();
#endif
};

#endif // SOURCE_UPDATER_TEST_H
