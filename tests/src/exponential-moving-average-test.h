#ifndef EXPONENTIAL_MOVING_AVERAGE_TEST_H
#define EXPONENTIAL_MOVING_AVERAGE_TEST_H

#include "test-suite.h"


class ExponentialMovingAverageTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testEmpty();
		void testClear();
		void testFirstValue();
		void testBasic();
		void testSetSmoothingFactor();
};

#endif // EXPONENTIAL_MOVING_AVERAGE_TEST_H
