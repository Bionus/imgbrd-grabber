#include "tag-name-test.h"
#include <QtTest>
#include "tags/tag-name.h"


void TagNameTest::testNormalizedValid()
{
	TagNameFormat capsSpace(TagNameFormat::Caps, " ");
	TagNameFormat upperFirstDash(TagNameFormat::UpperFirst, "-");

	QCOMPARE(TagName("tag_name").normalized(), QString("tag_name"));
	QCOMPARE(TagName("TAG NAME", capsSpace).normalized(), QString("tag_name"));
	QCOMPARE(TagName("Tag-name", upperFirstDash).normalized(), QString("tag_name"));
}

void TagNameTest::testNormalizedInvalid()
{
	QCOMPARE(TagName("TAG NAME").normalized(), QString("TAG NAME"));
}

void TagNameTest::testFormatted()
{
	TagNameFormat capsSpace(TagNameFormat::Caps, " ");
	TagNameFormat upperFirstDash(TagNameFormat::UpperFirst, "-");

	QCOMPARE(TagName("tag_name").formatted(capsSpace), QString("TAG NAME"));
	QCOMPARE(TagName("tag_name").formatted(upperFirstDash), QString("Tag-name"));
	QCOMPARE(TagName("Tag-name", upperFirstDash).formatted(capsSpace), QString("TAG NAME"));
	QCOMPARE(TagName("TAG NAME", capsSpace).formatted(upperFirstDash), QString("Tag-name"));
}

void TagNameTest::testCompare()
{
	TagNameFormat capsSpace(TagNameFormat::Caps, " ");
	TagNameFormat upperFirstDash(TagNameFormat::UpperFirst, "-");

	// Valid
	QCOMPARE(TagName("Tag-name", upperFirstDash) == TagName("tag_name"), true);
	QCOMPARE(TagName("TAG NAME", capsSpace) == TagName("tag_name"), true);
	QCOMPARE(TagName("Tag-name", upperFirstDash) == TagName("TAG NAME", capsSpace), true);

	// Invalid
	QCOMPARE(TagName("Tag-name-2", upperFirstDash) == TagName("tag_name"), false);
	QCOMPARE(TagName("TAG NAME 2", capsSpace) == TagName("tag_name"), false);
	QCOMPARE(TagName("Tag-name 2", upperFirstDash) == TagName("TAG NAME", capsSpace), false);
}


QTEST_MAIN(TagNameTest)
