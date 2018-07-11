#ifndef TAG_FILTER_TEST_H
#define TAG_FILTER_TEST_H

#include "test-suite.h"


class TagFilterTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testToString();
		void testCompare();
		void testMatchExact();
		void testMatchWildcard();
};

#endif // TAG_FILTER_TEST_H
