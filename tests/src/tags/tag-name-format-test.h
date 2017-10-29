#ifndef TAG_NAME_FORMAT_TEST_H
#define TAG_NAME_FORMAT_TEST_H

#include "test-suite.h"
#include "tags/tag-name-format.h"


class TagNameFormatTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testLower();
		void testUpperFirst();
		void testUpper();
		void testCaps();
};

#endif // TAG_NAME_FORMAT_TEST_H
