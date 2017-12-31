#ifndef SOURCE_GUESSER_TEST_H
#define SOURCE_GUESSER_TEST_H

#include "test-suite.h"


class SourceGuesserTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void initTestCase();

		void testNotFound();
		void testNetworkError();
		void testDanbooru1();
		void testDanbooru2();
};

#endif // SOURCE_GUESSER_TEST_H
