#include "token-test.h"
#include <QtTest>
#include "loader/token.h"


void TokenTest::testLazyNotCalled()
{
	int callCount = 0;
	Token token([&callCount]() { return ++callCount; });

	QCOMPARE(callCount, 0);
}

void TokenTest::testLazyWithCaching()
{
	int callCount = 0;
	Token token([&callCount]() { return ++callCount; }, true);

	token.value();
	int val = token.value().toInt();

	QCOMPARE(callCount, 1);
	QCOMPARE(val, 1);
}

void TokenTest::testLazyWithoutCaching()
{
	int callCount = 0;
	Token token([&callCount]() { return ++callCount; }, false);

	token.value();
	int val = token.value().toInt();

	QCOMPARE(callCount, 2);
	QCOMPARE(val, 2);
}


QTEST_MAIN(TokenTest)
