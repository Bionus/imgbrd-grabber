#ifndef TOKEN_TEST_H
#define TOKEN_TEST_H

#include "test-suite.h"


class TokenTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testLazyNotCalled();
		void testLazyWithCaching();
		void testLazyWithoutCaching();
};

#endif // TOKEN_TEST_H
