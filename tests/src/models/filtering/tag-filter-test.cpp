#include "loader/token.h"
#include "models/filtering/tag-filter.h"
#include "models/filtering/token-filter.h"
#include "catch.h"


TEST_CASE("TagFilter")
{
	SECTION("ToString")
	{
		REQUIRE(TagFilter("test").toString() == QString("test"));
		REQUIRE(TagFilter("test", true).toString() == QString("-test"));
	}

	SECTION("Compare")
	{
		REQUIRE(TagFilter("test") == TagFilter("test"));
		REQUIRE(TagFilter("test") != TagFilter("test", true));
		REQUIRE(TagFilter("test") != TagFilter("another test"));
		REQUIRE(TagFilter("test") != TokenFilter("token"));
	}

	SECTION("MatchExact")
	{
		QMap<QString, Token> tokens;
		tokens.insert("allos", Token(QStringList() << "ok" << "ok2"));

		// Basic
		REQUIRE(TagFilter("ok").match(tokens) == QString());
		REQUIRE(TagFilter("nok").match(tokens) == QString("image does not contains \"nok\""));

		// Invert
		REQUIRE(TagFilter("ok", true).match(tokens) == QString("image contains \"ok\""));
		REQUIRE(TagFilter("nok", true).match(tokens) == QString());
	}

	SECTION("MatchWildcard")
	{
		QMap<QString, Token> tokens;
		tokens.insert("allos", Token(QStringList() << "abc" << "bcd" << "cde", "def"));

		// Basic
		REQUIRE(TagFilter("bc*").match(tokens) == QString());
		REQUIRE(TagFilter("ef*").match(tokens) == QString("image does not contains \"ef*\""));

		// Invert
		REQUIRE(TagFilter("bc*", true).match(tokens) == QString("image contains \"bc*\""));
		REQUIRE(TagFilter("ef*", true).match(tokens) == QString());
	}
}
