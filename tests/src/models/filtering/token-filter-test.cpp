#include "token-filter-test.h"
#include <QtTest>
#include "loader/token.h"
#include "models/filtering/tag-filter.h"
#include "models/filtering/token-filter.h"


void TokenFilterTest::testToString()
{
	QCOMPARE(TokenFilter("test").toString(), QString("%test%"));
	QCOMPARE(TokenFilter("test", true).toString(), QString("-%test%"));
}

void TokenFilterTest::testCompare()
{
	QCOMPARE(TokenFilter("test") == TokenFilter("test"), true);
	QCOMPARE(TokenFilter("test") == TokenFilter("test", true), false);
	QCOMPARE(TokenFilter("test") == TokenFilter("another test"), false);
	QCOMPARE(TokenFilter("test") == TagFilter("tag"), false);
}

void TokenFilterTest::testMatchInt()
{
	QMap<QString, Token> tokens;
	tokens.insert("ok", Token(1));
	tokens.insert("nok", Token(QVariant(0)));

	// Basic
	QCOMPARE(TokenFilter("ok").match(tokens), QString());
	QCOMPARE(TokenFilter("nok").match(tokens), QString("image does not have a \"nok\" token"));
	QCOMPARE(TokenFilter("not_found").match(tokens), QString("image does not have a \"not_found\" token"));

	// Invert
	QCOMPARE(TokenFilter("ok", true).match(tokens), QString("image has a \"ok\" token"));
	QCOMPARE(TokenFilter("nok", true).match(tokens), QString());
	QCOMPARE(TokenFilter("not_found", true).match(tokens), QString());
}

void TokenFilterTest::testMatchString()
{
	QMap<QString, Token> tokens;
	tokens.insert("ok", Token("ok"));
	tokens.insert("nok", Token(""));

	// Basic
	QCOMPARE(TokenFilter("ok").match(tokens), QString());
	QCOMPARE(TokenFilter("nok").match(tokens), QString("image does not have a \"nok\" token"));
	QCOMPARE(TokenFilter("not_found").match(tokens), QString("image does not have a \"not_found\" token"));

	// Invert
	QCOMPARE(TokenFilter("ok", true).match(tokens), QString("image has a \"ok\" token"));
	QCOMPARE(TokenFilter("nok", true).match(tokens), QString());
	QCOMPARE(TokenFilter("not_found", true).match(tokens), QString());
}

void TokenFilterTest::testMatchStringList()
{
	QMap<QString, Token> tokens;
	tokens.insert("ok", Token(QStringList() << "ok"));
	tokens.insert("nok", Token(QStringList()));

	// Basic
	QCOMPARE(TokenFilter("ok").match(tokens), QString());
	QCOMPARE(TokenFilter("nok").match(tokens), QString("image does not have a \"nok\" token"));
	QCOMPARE(TokenFilter("not_found").match(tokens), QString("image does not have a \"not_found\" token"));

	// Invert
	QCOMPARE(TokenFilter("ok", true).match(tokens), QString("image has a \"ok\" token"));
	QCOMPARE(TokenFilter("nok", true).match(tokens), QString());
	QCOMPARE(TokenFilter("not_found", true).match(tokens), QString());
}


QTEST_MAIN(TokenFilterTest)
