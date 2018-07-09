#ifndef TAG_FILTER_TEST_H
#define TAG_FILTER_TEST_H

#include "test-suite.h"


class TagFilterTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testToString();
		void testMatch();
};

#endif // TAG_FILTER_TEST_H
