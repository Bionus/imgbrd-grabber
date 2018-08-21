#ifndef TAG_NAME_TEST_H
#define TAG_NAME_TEST_H

#include "test-suite.h"


class TagNameTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testNormalizedValid();
		void testNormalizedInvalid();
		void testFormatted();
		void testCompare();
};

#endif // TAG_NAME_TEST_H
