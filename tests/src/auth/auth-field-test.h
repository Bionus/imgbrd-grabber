#ifndef AUTH_FIELD_TEST_H
#define AUTH_FIELD_TEST_H

#include "test-suite.h"


class AuthFieldTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testBasic();
		void testConst();
		void testHash();
};

#endif // AUTH_FIELD_TEST_H
