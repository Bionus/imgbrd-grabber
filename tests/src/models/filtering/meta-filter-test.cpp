#include <QDate>
#include <QDateTime>
#include "loader/token.h"
#include "models/filtering/meta-filter.h"
#include "models/filtering/tag-filter.h"
#include "catch.h"


TEST_CASE("MetaFilter")
{
	SECTION("ToString")
	{
		REQUIRE(MetaFilter("meta", "val").toString() == QString("meta:val"));
		REQUIRE(MetaFilter("meta", "val", true).toString() == QString("-meta:val"));
	}

	SECTION("Compare")
	{
		REQUIRE(MetaFilter("meta", "val") == MetaFilter("meta", "val"));
		REQUIRE(MetaFilter("meta", "val") != MetaFilter("meta", "val", true));
		REQUIRE(MetaFilter("meta", "val") != MetaFilter("another meta", "val"));
		REQUIRE(MetaFilter("meta", "val") != MetaFilter("meta", "another val"));
		REQUIRE(MetaFilter("meta", "val") != TagFilter("tag"));
	}

	SECTION("MatchInvalidToken")
	{
		QMap<QString, Token> tokens;
		tokens.insert("token_1", Token(1));
		tokens.insert("token_2", Token(2));

		QString expected = "unknown type \"not_found\" (available types: \"token_1\", \"token_2\")";

		REQUIRE(MetaFilter("not_found", "val").match(tokens) == expected);
		REQUIRE(MetaFilter("not_found", "val", true).match(tokens) == expected);
	}

	SECTION("MatchGrabber")
	{
		QMap<QString, Token> tokens;
		tokens.insert("grabber", Token(QStringList() << "downloaded"));

		// Basic
		REQUIRE(MetaFilter("grabber", "downloaded").match(tokens) == QString());
		REQUIRE(MetaFilter("grabber", "nok").match(tokens) == QString("image is not \"nok\""));

		// Invert
		REQUIRE(MetaFilter("grabber", "downloaded", true).match(tokens) == QString("image is \"downloaded\""));
		REQUIRE(MetaFilter("grabber", "nok", true).match(tokens) == QString());
	}

	SECTION("MatchMathematical")
	{
		QMap<QString, Token> tokens;
		tokens.insert("id", Token(12345));

		// Basic
		REQUIRE(MetaFilter("id", ">1000").match(tokens) == QString());
		REQUIRE(MetaFilter("id", ">=1000").match(tokens) == QString());
		REQUIRE(MetaFilter("id", "<1000").match(tokens) == QString("image's id does not match"));
		REQUIRE(MetaFilter("id", "<=1000").match(tokens) == QString("image's id does not match"));
		REQUIRE(MetaFilter("id", "1000..").match(tokens) == QString());
		REQUIRE(MetaFilter("id", "..1000").match(tokens) == QString("image's id does not match"));
		REQUIRE(MetaFilter("id", "10000..20000").match(tokens) == QString());
		REQUIRE(MetaFilter("id", "10").match(tokens) == QString("image's id does not match"));

		// Invert
		REQUIRE(MetaFilter("id", ">1000", true).match(tokens) == QString("image's id match"));
		REQUIRE(MetaFilter("id", ">=1000", true).match(tokens) == QString("image's id match"));
		REQUIRE(MetaFilter("id", "<1000", true).match(tokens) == QString());
		REQUIRE(MetaFilter("id", "<=1000", true).match(tokens) == QString());
		REQUIRE(MetaFilter("id", "1000..", true).match(tokens) == QString("image's id match"));
		REQUIRE(MetaFilter("id", "..1000", true).match(tokens) == QString());
		REQUIRE(MetaFilter("id", "10000..20000", true).match(tokens) == QString("image's id match"));
		REQUIRE(MetaFilter("id", "10", true).match(tokens) == QString());
	}

	SECTION("MatchDate")
	{
		QMap<QString, Token> tokens;
		tokens.insert("date", Token(QDateTime(QDate(2016, 8, 18))));

		REQUIRE(MetaFilter("date", ">08/16/2016").match(tokens) == QString());
		REQUIRE(MetaFilter("date", ">=2016-08-16").match(tokens) == QString());
		REQUIRE(MetaFilter("date", "<08/20/2016").match(tokens) == QString());
		REQUIRE(MetaFilter("date", "<=2016-08-20").match(tokens) == QString());
		REQUIRE(MetaFilter("date", "..08/20/2016").match(tokens) == QString());
		REQUIRE(MetaFilter("date", "2016-08-16..").match(tokens) == QString());
		REQUIRE(MetaFilter("date", "08/16/2016..2016-08-20").match(tokens) == QString());
		REQUIRE(MetaFilter("date", "2016-08-18").match(tokens) == QString());

		// Invalid date
		REQUIRE(MetaFilter("date", "someday").match(tokens) == QString("image's date does not match"));
	}

	SECTION("MatchRating")
	{
		QMap<QString, Token> tokens;
		tokens.insert("rating", Token("safe"));

		// Basic
		REQUIRE(MetaFilter("rating", "s").match(tokens) == QString());
		REQUIRE(MetaFilter("rating", "safe").match(tokens) == QString());
		REQUIRE(MetaFilter("rating", "e").match(tokens) == QString("image is not \"explicit\""));
		REQUIRE(MetaFilter("rating", "explicit").match(tokens) == QString("image is not \"explicit\""));

		// Invert
		REQUIRE(MetaFilter("rating", "s", true).match(tokens) == QString("image is \"safe\""));
		REQUIRE(MetaFilter("rating", "safe", true).match(tokens) == QString("image is \"safe\""));
		REQUIRE(MetaFilter("rating", "e", true).match(tokens) == QString());
		REQUIRE(MetaFilter("rating", "explicit", true).match(tokens) == QString());
	}

	SECTION("MatchSource")
	{
		QMap<QString, Token> tokens;
		tokens.insert("source", Token("test.com/some/path"));

		// Basic
		REQUIRE(MetaFilter("source", "test.com").match(tokens) == QString());
		REQUIRE(MetaFilter("source", "nok.com").match(tokens) == QString("image's source does not starts with \"nok.com\""));

		// Invert
		REQUIRE(MetaFilter("source", "test.com", true).match(tokens) == QString("image's source starts with \"test.com\""));
		REQUIRE(MetaFilter("source", "nok.com", true).match(tokens) == QString());
	}

	SECTION("MatchString")
	{
		QMap<QString, Token> tokens;
		tokens.insert("meta", Token("val"));

		// Basic
		REQUIRE(MetaFilter("meta", "val").match(tokens) == QString());
		REQUIRE(MetaFilter("meta", "nok").match(tokens) == QString("image's meta does not match"));

		// Invert
		REQUIRE(MetaFilter("meta", "val", true).match(tokens) == QString("image's meta match"));
		REQUIRE(MetaFilter("meta", "nok", true).match(tokens) == QString());
	}

	SECTION("MatchAge")
	{
		QMap<QString, Token> tokens;
		REQUIRE(MetaFilter("age", "1year..1day").match(tokens) == QString("An image needs a date to be filtered by age"));

		tokens.insert("date", Token(QDateTime(QDate(2016, 8, 18))));
		tokens.insert("TESTS_now", Token(QDateTime(QDate(2016, 10, 16))));

		// Basic
		REQUIRE(MetaFilter("age", ">=2seconds").match(tokens) == QString());
		REQUIRE(MetaFilter("age", ">=2mi").match(tokens) == QString());
		REQUIRE(MetaFilter("age", ">=2hours").match(tokens) == QString());
		REQUIRE(MetaFilter("age", ">1day").match(tokens) == QString());
		REQUIRE(MetaFilter("age", ">1w").match(tokens) == QString());
		REQUIRE(MetaFilter("age", ">1mo").match(tokens) == QString());
		REQUIRE(MetaFilter("age", ">=1y").match(tokens) == QString("image's age does not match"));
		REQUIRE(MetaFilter("age", "<1year").match(tokens) == QString());

		// Invert
		REQUIRE(MetaFilter("age", ">=1y", true).match(tokens) == QString());
		REQUIRE(MetaFilter("age", "<1year", true).match(tokens) == QString("image's age match"));
	}
}
