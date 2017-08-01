#ifndef SOURCE_UPDATER_TEST_H
#define SOURCE_UPDATER_TEST_H

#include "test-suite.h"


class SourceUpdaterTest : public TestSuite
{
	Q_OBJECT

	private slots:
#ifdef Q_OS_WIN
		void testNoUpdate();
		void testChanged();
#endif
};

#endif // SOURCE_UPDATER_TEST_H
