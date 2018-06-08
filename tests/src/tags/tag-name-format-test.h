#ifndef TAG_NAME_FORMAT_TEST_H
#define TAG_NAME_FORMAT_TEST_H

#include "tags/tag-name-format.h"
#include "test-suite.h"


class TagNameFormatTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testLower();
		void testUpperFirst();
		void testUpper();
		void testCaps();
		void testUnknown();
};

#endif // TAG_NAME_FORMAT_TEST_H
