#ifndef SOURCE_GUESSER_TEST_H
#define SOURCE_GUESSER_TEST_H

#include "test-suite.h"


class Profile;

class SourceGuesserTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanupTestCase();

		void testNotFound();
		void testNetworkError();
		void testDanbooru1();
		void testDanbooru2();

	private:
		Profile *m_profile;
};

#endif // SOURCE_GUESSER_TEST_H
