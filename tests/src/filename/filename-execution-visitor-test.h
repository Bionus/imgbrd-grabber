#ifndef FILENAME_EXECUTION_VISITOR_TEST_H
#define FILENAME_EXECUTION_VISITOR_TEST_H

#include "test-suite.h"


class FilenameExecutionVisitorTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testEmpty();
		void testBasic();
		void testToken();
};

#endif // FILENAME_EXECUTION_VISITOR_TEST_H
