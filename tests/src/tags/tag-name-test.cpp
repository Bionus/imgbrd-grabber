#include "tag-name-test.h"
#include <QtTest>


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


static TagNameTest instance;
