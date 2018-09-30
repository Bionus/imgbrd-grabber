#ifndef SEARCH_FORMAT_VISITOR_TEST_H
#define SEARCH_FORMAT_VISITOR_TEST_H

#include "test-suite.h"


class SearchFormatVisitorTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testOrOnly();
		void testOrAnd();
		void testPrefix();
};

#endif // SEARCH_FORMAT_VISITOR_TEST_H
