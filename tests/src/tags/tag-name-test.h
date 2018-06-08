#ifndef TAG_NAME_TEST_H
#define TAG_NAME_TEST_H

#include "tags/tag-name.h"
#include "test-suite.h"


class TagNameTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testNormalizedValid();
		void testNormalizedInvalid();
		void testFormatted();
};

#endif // TAG_NAME_TEST_H
