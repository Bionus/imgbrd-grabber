#ifndef TAG_NAME_TEST_H
#define TAG_NAME_TEST_H

#include "test-suite.h"
#include "tags/tag-name.h"


class TagNameTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testNormalizedValid();
		void testNormalizedInvalid();
		void testFormatted();
};

#endif // TAG_NAME_TEST_H
