#include <QtTest>
#include "pool-test.h"


void PoolTest::testGetId()
{
	Pool pool(123, "Test pool", 1, 2, 3);
	QCOMPARE(pool.id(), 123);
}
void PoolTest::testGetName()
{
	Pool pool(123, "Test pool", 1, 2, 3);
	QCOMPARE(pool.name(), QString("Test pool"));
}
void PoolTest::testGetCurrent()
{
	Pool pool(123, "Test pool", 1, 2, 3);
	QCOMPARE(pool.current(), 1);
}
void PoolTest::testGetNext()
{
	Pool pool(123, "Test pool", 1, 2, 3);
	QCOMPARE(pool.next(), 2);
}
void PoolTest::testGetPrevious()
{
	Pool pool(123, "Test pool", 1, 2, 3);
	QCOMPARE(pool.previous(), 3);
}

static PoolTest instance;
