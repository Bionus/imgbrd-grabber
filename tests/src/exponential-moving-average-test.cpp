#include "exponential-moving-average-test.h"
#include <QtTest>
#include "exponential-moving-average.h"


void ExponentialMovingAverageTest::testEmpty()
{
	ExponentialMovingAverage avg(0.5);

	QCOMPARE(avg.average(), 0.0);
}

void ExponentialMovingAverageTest::testClear()
{
	ExponentialMovingAverage avg(0.5);
	avg.addValue(1);
	avg.clear();

	QCOMPARE(avg.average(), 0.0);
}

void ExponentialMovingAverageTest::testFirstValue()
{
	ExponentialMovingAverage avg(0.5);
	avg.addValue(1);

	QCOMPARE(avg.average(), 1.0);
}

void ExponentialMovingAverageTest::testBasic()
{
	ExponentialMovingAverage avg(0.5);
	avg.addValue(2);
	avg.addValue(4);
	avg.addValue(5);

	QCOMPARE(avg.average(), 4.0);
}

void ExponentialMovingAverageTest::testSetSmoothingFactor()
{
	ExponentialMovingAverage avg(1);
	avg.setSmoothingFactor(0.5);
	avg.addValue(2);
	avg.addValue(4);
	avg.addValue(5);

	QCOMPARE(avg.average(), 4.0);
}


QTEST_MAIN(ExponentialMovingAverageTest)
