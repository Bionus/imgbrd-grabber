#include "extension-rotator-test.h"
#include <QtTest>
#include <algorithm>


void ExtensionRotatorTest::testBasic()
{
	ExtensionRotator rotator("jpg", QStringList() << "jpg" << "png" << "gif");

	QCOMPARE(rotator.next(), QString("png"));
	QCOMPARE(rotator.next(), QString("gif"));
	QCOMPARE(rotator.next(), QString());
}

void ExtensionRotatorTest::testLoop()
{
	ExtensionRotator rotator("png", QStringList() << "jpg" << "png" << "gif");

	QCOMPARE(rotator.next(), QString("gif"));
	QCOMPARE(rotator.next(), QString("jpg"));
	QCOMPARE(rotator.next(), QString());
}

void ExtensionRotatorTest::testNotFound()
{
	ExtensionRotator rotator("mp4", QStringList() << "jpg" << "png" << "gif");

	QCOMPARE(rotator.next(), QString("jpg"));
	QCOMPARE(rotator.next(), QString("png"));
	QCOMPARE(rotator.next(), QString("gif"));
	QCOMPARE(rotator.next(), QString());
}

void ExtensionRotatorTest::testKeepEmpty()
{
	ExtensionRotator rotator("jpg", QStringList() << "jpg" << "png" << "gif");

	QCOMPARE(rotator.next(), QString("png"));
	QCOMPARE(rotator.next(), QString("gif"));
	QCOMPARE(rotator.next(), QString());
	QCOMPARE(rotator.next(), QString());
}

void ExtensionRotatorTest::testEmptyFirst()
{
	ExtensionRotator rotator("", QStringList() << "jpg" << "png" << "gif");

	QCOMPARE(rotator.next(), QString("jpg"));
	QCOMPARE(rotator.next(), QString("png"));
	QCOMPARE(rotator.next(), QString("gif"));
	QCOMPARE(rotator.next(), QString());
}

void ExtensionRotatorTest::testEmptyList()
{
	ExtensionRotator rotator("jpg", QStringList());

	QCOMPARE(rotator.next(), QString());
}

void ExtensionRotatorTest::testEmptyBoth()
{
	ExtensionRotator rotator("", QStringList());

	QCOMPARE(rotator.next(), QString());
}

void ExtensionRotatorTest::testCopyConstructor()
{
	ExtensionRotator rotator("mp4", QStringList() << "jpg" << "png" << "gif");
	QCOMPARE(rotator.next(), QString("jpg"));
	QCOMPARE(rotator.next(), QString("png"));

	ExtensionRotator copy(rotator);
	QCOMPARE(copy.next(), QString("gif"));
	QCOMPARE(copy.next(), QString());
}


QTEST_MAIN(ExtensionRotatorTest)
