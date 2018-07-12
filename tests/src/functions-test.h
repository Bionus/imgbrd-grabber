#ifndef FUNCTIONS_TEST_H
#define FUNCTIONS_TEST_H

#include "test-suite.h"


class FunctionsTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testCopyRecursively();
		void testFixFilenameWindows();
		void testFixFilenameLinux();
		void testFontToCss();
		void testIsVariantEmpty();
		void testGetUnit();
		void testFormatFilesize();
		void testGetExtension();
		void testSetExtension();
		void testLevenshtein();
		void testRemoveWildards();
		void testDateTimeFromString();
		void testIsUrl();
		void testParseMarkdownHeaders();
		void testParseMarkdownIssueLinks();
		void testSetFileCreationDate();
		void testSetFileCreationDateUtf8();

	protected:
		void assertFixFilename(int platform, const QString &filename, const QString &path, const QString &expected);
};

#endif // FUNCTIONS_TEST_H
