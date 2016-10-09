#ifndef FUNCTIONS_TEST_H
#define FUNCTIONS_TEST_H

#include "test-suite.h"


class FunctionsTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testFixFilenameWindows();
		void testFixFilenameLinux();
		void testGetUnit();
		void testGetExtension();
		void testSetExtension();
		void testLevenshtein();

	protected:
		void assertFixFilename(int platform, QString filename, QString path, QString expected);
};

#endif // FUNCTIONS_TEST_H
