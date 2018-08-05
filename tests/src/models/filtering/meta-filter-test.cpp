#include "meta-filter-test.h"
#include <QtTest>
#include "models/filtering/meta-filter.h"
#include "loader/token.h"


void MetaFilterTest::testToString()
{
	QCOMPARE(MetaFilter("meta", "val").toString(), QString("meta:val"));
	QCOMPARE(MetaFilter("meta", "val", true).toString(), QString("-meta:val"));
}

void MetaFilterTest::testCompare()
{
	QCOMPARE(MetaFilter("meta", "val") == MetaFilter("meta", "val"), true);
	QCOMPARE(MetaFilter("meta", "val") == MetaFilter("meta", "val", true), false);
	QCOMPARE(MetaFilter("meta", "val") == MetaFilter("another meta", "val"), false);
	QCOMPARE(MetaFilter("meta", "val") == MetaFilter("meta", "another val"), false);
}

void MetaFilterTest::testMatchInvalidToken()
{
	QMap<QString, Token> tokens;
	tokens.insert("token_1", Token(1));
	tokens.insert("token_2", Token(2));

	QString expected = "unknown type \"not_found\" (available types: \"token_1\", \"token_2\")";

	QCOMPARE(MetaFilter("not_found", "val").match(tokens), expected);
	QCOMPARE(MetaFilter("not_found", "val", true).match(tokens), expected);
}

void MetaFilterTest::testMatchGrabber()
{
	QMap<QString, Token> tokens;
	tokens.insert("grabber", Token(QStringList() << "downloaded"));

	// Basic
	QCOMPARE(MetaFilter("grabber", "downloaded").match(tokens), QString());
	QCOMPARE(MetaFilter("grabber", "nok").match(tokens), QString("image is not \"nok\""));

	// Invert
	QCOMPARE(MetaFilter("grabber", "downloaded", true).match(tokens), QString("image is \"downloaded\""));
	QCOMPARE(MetaFilter("grabber", "nok", true).match(tokens), QString());
}

void MetaFilterTest::testMatchMathematical()
{
	QMap<QString, Token> tokens;
	tokens.insert("id", Token(12345));

	// Basic
	QCOMPARE(MetaFilter("id", ">1000").match(tokens), QString());
	QCOMPARE(MetaFilter("id", ">=1000").match(tokens), QString());
	QCOMPARE(MetaFilter("id", "<1000").match(tokens), QString("image's id does not match"));
	QCOMPARE(MetaFilter("id", "<=1000").match(tokens), QString("image's id does not match"));
	QCOMPARE(MetaFilter("id", "1000..").match(tokens), QString());
	QCOMPARE(MetaFilter("id", "..1000").match(tokens), QString("image's id does not match"));
	QCOMPARE(MetaFilter("id", "10000..20000").match(tokens), QString());
	QCOMPARE(MetaFilter("id", "10").match(tokens), QString("image's id does not match"));

	// Invert
	QCOMPARE(MetaFilter("id", ">1000", true).match(tokens), QString("image's id match"));
	QCOMPARE(MetaFilter("id", ">=1000", true).match(tokens), QString("image's id match"));
	QCOMPARE(MetaFilter("id", "<1000", true).match(tokens), QString());
	QCOMPARE(MetaFilter("id", "<=1000", true).match(tokens), QString());
	QCOMPARE(MetaFilter("id", "1000..", true).match(tokens), QString("image's id match"));
	QCOMPARE(MetaFilter("id", "..1000", true).match(tokens), QString());
	QCOMPARE(MetaFilter("id", "10000..20000", true).match(tokens), QString("image's id match"));
	QCOMPARE(MetaFilter("id", "10", true).match(tokens), QString());
}

void MetaFilterTest::testMatchDate()
{
	QMap<QString, Token> tokens;
	tokens.insert("date", Token(QDateTime(QDate(2016, 8, 18))));

	QCOMPARE(MetaFilter("date", ">08/16/2016").match(tokens), QString());
	QCOMPARE(MetaFilter("date", ">=2016-08-16").match(tokens), QString());
	QCOMPARE(MetaFilter("date", "<08/20/2016").match(tokens), QString());
	QCOMPARE(MetaFilter("date", "<=2016-08-20").match(tokens), QString());
	QCOMPARE(MetaFilter("date", "..08/20/2016").match(tokens), QString());
	QCOMPARE(MetaFilter("date", "2016-08-16..").match(tokens), QString());
	QCOMPARE(MetaFilter("date", "08/16/2016..2016-08-20").match(tokens), QString());
	QCOMPARE(MetaFilter("date", "2016-08-18").match(tokens), QString());

	// Invalid date
	QCOMPARE(MetaFilter("date", "someday").match(tokens), QString("image's date does not match"));
}

void MetaFilterTest::testMatchRating()
{
	QMap<QString, Token> tokens;
	tokens.insert("rating", Token("safe"));

	// Basic
	QCOMPARE(MetaFilter("rating", "s").match(tokens), QString());
	QCOMPARE(MetaFilter("rating", "safe").match(tokens), QString());
	QCOMPARE(MetaFilter("rating", "e").match(tokens), QString("image is not \"explicit\""));
	QCOMPARE(MetaFilter("rating", "explicit").match(tokens), QString("image is not \"explicit\""));

	// Invert
	QCOMPARE(MetaFilter("rating", "s", true).match(tokens), QString("image is \"safe\""));
	QCOMPARE(MetaFilter("rating", "safe", true).match(tokens), QString("image is \"safe\""));
	QCOMPARE(MetaFilter("rating", "e", true).match(tokens), QString());
	QCOMPARE(MetaFilter("rating", "explicit", true).match(tokens), QString());
}

void MetaFilterTest::testMatchSource()
{
	QMap<QString, Token> tokens;
	tokens.insert("source", Token("test.com/some/path"));

	// Basic
	QCOMPARE(MetaFilter("source", "test.com").match(tokens), QString());
	QCOMPARE(MetaFilter("source", "nok.com").match(tokens), QString("image's source does not starts with \"nok.com\""));

	// Invert
	QCOMPARE(MetaFilter("source", "test.com", true).match(tokens), QString("image's source starts with \"test.com\""));
	QCOMPARE(MetaFilter("source", "nok.com", true).match(tokens), QString());
}

void MetaFilterTest::testMatchString()
{
	QMap<QString, Token> tokens;
	tokens.insert("meta", Token("val"));

	// Basic
	QCOMPARE(MetaFilter("meta", "val").match(tokens), QString());
	QCOMPARE(MetaFilter("meta", "nok").match(tokens), QString("image's meta does not match"));

	// Invert
	QCOMPARE(MetaFilter("meta", "val", true).match(tokens), QString("image's meta match"));
	QCOMPARE(MetaFilter("meta", "nok", true).match(tokens), QString());
}

void MetaFilterTest::testMatchAge()
{
	QMap<QString, Token> tokens;
	QCOMPARE(MetaFilter("age", "1year..1day").match(tokens), QString("An image needs a date to be filtered by age"));

	tokens.insert("date", Token(QDateTime(QDate(2016, 8, 18))));
	tokens.insert("TESTS_now", Token(QDateTime(QDate(2016, 10, 16))));

	// Basic
	QCOMPARE(MetaFilter("age", ">=2hours").match(tokens), QString());
	QCOMPARE(MetaFilter("age", ">1day").match(tokens), QString());
	QCOMPARE(MetaFilter("age", ">1mo").match(tokens), QString());
	QCOMPARE(MetaFilter("age", ">=1y").match(tokens), QString("image's age does not match"));
	QCOMPARE(MetaFilter("age", "<1year").match(tokens), QString());

	// Invert
	QCOMPARE(MetaFilter("age", ">=1y", true).match(tokens), QString());
	QCOMPARE(MetaFilter("age", "<1year", true).match(tokens), QString("image's age match"));
}


QTEST_MAIN(MetaFilterTest)
