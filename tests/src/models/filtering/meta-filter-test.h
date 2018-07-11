#ifndef META_FILTER_TEST_H
#define META_FILTER_TEST_H

#include "test-suite.h"


class MetaFilterTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testToString();
		void testCompare();
		void testMatchInvalidToken();
		void testMatchGrabber();
		void testMatchMathematical();
		void testMatchDate();
		void testMatchRating();
		void testMatchSource();
		void testMatchString();
};

#endif // META_FILTER_TEST_H
