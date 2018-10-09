#ifndef LANGUAGE_LOADER_TEST_H
#define LANGUAGE_LOADER_TEST_H

#include "test-suite.h"


class LanguageLoaderTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testInvalid();
		void testValid();
};

#endif // LANGUAGE_LOADER_TEST_H
