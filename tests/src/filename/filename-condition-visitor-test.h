#ifndef FILENAME_CONDITION_VISITOR_TEST_H
#define FILENAME_CONDITION_VISITOR_TEST_H

#include "test-suite.h"


class FilenameConditionVisitorTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testTag();
		void testToken();
		void testOperatorOr();
		void testOperatorAnd();
		void testMixedOperators();
		void testInvert();
};

#endif // FILENAME_CONDITION_VISITOR_TEST_H
