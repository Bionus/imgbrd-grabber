#include "loader/token.h"
#include "models/filtering/tag-filter.h"
#include "models/filtering/token-filter.h"
#include "catch.h"


TEST_CASE("TokenFilter")
{
	SECTION("ToString")
	{
		REQUIRE(TokenFilter("test").toString() == QString("%test%"));
		REQUIRE(TokenFilter("test", true).toString() == QString("-%test%"));
	}

	SECTION("Compare")
	{
		REQUIRE(TokenFilter("test") == TokenFilter("test"));
		REQUIRE(TokenFilter("test") != TokenFilter("test", true));
		REQUIRE(TokenFilter("test") != TokenFilter("another test"));
		REQUIRE(TokenFilter("test") != TagFilter("tag"));
	}

	SECTION("MatchInt")
	{
		QMap<QString, Token> tokens;
		tokens.insert("ok", Token(1));
		tokens.insert("nok", Token(QVariant(0)));

		// Basic
		REQUIRE(TokenFilter("ok").match(tokens) == QString());
		REQUIRE(TokenFilter("nok").match(tokens) == QString("image does not have a \"nok\" token"));
		REQUIRE(TokenFilter("not_found").match(tokens) == QString("image does not have a \"not_found\" token"));

		// Invert
		REQUIRE(TokenFilter("ok", true).match(tokens) == QString("image has a \"ok\" token"));
		REQUIRE(TokenFilter("nok", true).match(tokens) == QString());
		REQUIRE(TokenFilter("not_found", true).match(tokens) == QString());
	}

	SECTION("MatchString")
	{
		QMap<QString, Token> tokens;
		tokens.insert("ok", Token("ok"));
		tokens.insert("nok", Token(""));

		// Basic
		REQUIRE(TokenFilter("ok").match(tokens) == QString());
		REQUIRE(TokenFilter("nok").match(tokens) == QString("image does not have a \"nok\" token"));
		REQUIRE(TokenFilter("not_found").match(tokens) == QString("image does not have a \"not_found\" token"));

		// Invert
		REQUIRE(TokenFilter("ok", true).match(tokens) == QString("image has a \"ok\" token"));
		REQUIRE(TokenFilter("nok", true).match(tokens) == QString());
		REQUIRE(TokenFilter("not_found", true).match(tokens) == QString());
	}

	SECTION("MatchStringList")
	{
		QMap<QString, Token> tokens;
		tokens.insert("ok", Token(QStringList() << "ok"));
		tokens.insert("nok", Token(QStringList()));

		// Basic
		REQUIRE(TokenFilter("ok").match(tokens) == QString());
		REQUIRE(TokenFilter("nok").match(tokens) == QString("image does not have a \"nok\" token"));
		REQUIRE(TokenFilter("not_found").match(tokens) == QString("image does not have a \"not_found\" token"));

		// Invert
		REQUIRE(TokenFilter("ok", true).match(tokens) == QString("image has a \"ok\" token"));
		REQUIRE(TokenFilter("nok", true).match(tokens) == QString());
		REQUIRE(TokenFilter("not_found", true).match(tokens) == QString());
	}
}
