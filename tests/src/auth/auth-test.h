#ifndef AUTH_TEST_H
#define AUTH_TEST_H

#include "test-suite.h"


class AuthTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testUrlAuth();
		void testHttpAuth();
		void testOAuth2Auth();
};

#endif // AUTH_TEST_H
