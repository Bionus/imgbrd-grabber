#include "tag-name-format-test.h"
#include <QtTest>


void TagNameFormatTest::testLower()
{
	TagNameFormat format(TagNameFormat::Lower, "_");

	QCOMPARE(format.formatted(QStringList()), QString(""));
	QCOMPARE(format.formatted(QStringList() << "test"), QString("test"));
	QCOMPARE(format.formatted(QStringList() << "test" << "tag"), QString("test_tag"));
	QCOMPARE(format.formatted(QStringList() << "Test" << "tAG"), QString("test_tag"));
}

void TagNameFormatTest::testUpperFirst()
{
	TagNameFormat format(TagNameFormat::UpperFirst, "_");

	QCOMPARE(format.formatted(QStringList()), QString(""));
	QCOMPARE(format.formatted(QStringList() << "test"), QString("Test"));
	QCOMPARE(format.formatted(QStringList() << "test" << "tag"), QString("Test_tag"));
	QCOMPARE(format.formatted(QStringList() << "Test" << "tAG"), QString("Test_tag"));
}

void TagNameFormatTest::testUpper()
{
	TagNameFormat format(TagNameFormat::Upper, "_");

	QCOMPARE(format.formatted(QStringList()), QString(""));
	QCOMPARE(format.formatted(QStringList() << "test"), QString("Test"));
	QCOMPARE(format.formatted(QStringList() << "test" << "tag"), QString("Test_Tag"));
	QCOMPARE(format.formatted(QStringList() << "Test" << "tAG"), QString("Test_Tag"));
}

void TagNameFormatTest::testCaps()
{
	TagNameFormat format(TagNameFormat::Caps, "_");

	QCOMPARE(format.formatted(QStringList()), QString(""));
	QCOMPARE(format.formatted(QStringList() << "test"), QString("TEST"));
	QCOMPARE(format.formatted(QStringList() << "test" << "tag"), QString("TEST_TAG"));
	QCOMPARE(format.formatted(QStringList() << "Test" << "tAG"), QString("TEST_TAG"));
}

void TagNameFormatTest::testUnknown()
{
	TagNameFormat format((TagNameFormat::CaseFormat)123, " ");

	QCOMPARE(format.formatted(QStringList()), QString(""));
	QCOMPARE(format.formatted(QStringList() << "test"), QString("test"));
	QCOMPARE(format.formatted(QStringList() << "test" << "tag"), QString("test tag"));
	QCOMPARE(format.formatted(QStringList() << "Test" << "tAG"), QString("Test tAG"));
}


QTEST_MAIN(TagNameFormatTest)
