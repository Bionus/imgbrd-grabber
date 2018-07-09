#ifndef TOKEN_FILTER_TEST_H
#define TOKEN_FILTER_TEST_H

#include "test-suite.h"


class TokenFilterTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testToString();
		void testMatchInt();
		void testMatchString();
		void testMatchStringList();
};

#endif // TOKEN_FILTER_TEST_H
