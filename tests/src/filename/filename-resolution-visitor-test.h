#ifndef FILENAME_RESOLUTION_VISITOR_TEST_H
#define FILENAME_RESOLUTION_VISITOR_TEST_H

#include "test-suite.h"


class FilenameResolutionVisitorTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testEmpty();
		void testBasic();
		void testConditional();
		void testDuplicates();
};

#endif // FILENAME_RESOLUTION_VISITOR_TEST_H
