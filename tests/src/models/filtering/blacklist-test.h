#ifndef BLACKLIST_TEST_H
#define BLACKLIST_TEST_H

#include "test-suite.h"


class BlacklistTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testToString();
		void testContains();
		void testRemove();
		void testMatch();
};

#endif // BLACKLIST_TEST_H
