#include "tag-filter-test.h"
#include <QtTest>
#include "models/filtering/tag-filter.h"
#include "models/filtering/token-filter.h"
#include "loader/token.h"


void TagFilterTest::testToString()
{
	QCOMPARE(TagFilter("test").toString(), QString("test"));
	QCOMPARE(TagFilter("test", true).toString(), QString("-test"));
}

void TagFilterTest::testCompare()
{
	QCOMPARE(TagFilter("test") == TagFilter("test"), true);
	QCOMPARE(TagFilter("test") == TagFilter("test", true), false);
	QCOMPARE(TagFilter("test") == TagFilter("another test"), false);
	QCOMPARE(TagFilter("test") == TokenFilter("token"), false);
}

void TagFilterTest::testMatchExact()
{
	QMap<QString, Token> tokens;
	tokens.insert("allos", Token(QStringList() << "ok" << "ok2"));

	// Basic
	QCOMPARE(TagFilter("ok").match(tokens), QString());
	QCOMPARE(TagFilter("nok").match(tokens), QString("image does not contains \"nok\""));

	// Invert
	QCOMPARE(TagFilter("ok", true).match(tokens), QString("image contains \"ok\""));
	QCOMPARE(TagFilter("nok", true).match(tokens), QString());
}

void TagFilterTest::testMatchWildcard()
{
	QMap<QString, Token> tokens;
	tokens.insert("allos", Token(QStringList() << "abc" << "bcd" << "cde", "def"));

	// Basic
	QCOMPARE(TagFilter("bc*").match(tokens), QString());
	QCOMPARE(TagFilter("ef*").match(tokens), QString("image does not contains \"ef*\""));

	// Invert
	QCOMPARE(TagFilter("bc*", true).match(tokens), QString("image contains \"bc*\""));
	QCOMPARE(TagFilter("ef*", true).match(tokens), QString());
}


QTEST_MAIN(TagFilterTest)
