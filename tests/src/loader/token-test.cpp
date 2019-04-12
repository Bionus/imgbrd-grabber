#include "loader/token.h"
#include "catch.h"


TEST_CASE("Token")
{
	SECTION("LazyNotCalled")
	{
		int callCount = 0;
		Token token([&callCount]() { return ++callCount; });

		REQUIRE(callCount == 0);
	}

	SECTION("LazyWithCaching")
	{
		int callCount = 0;
		Token token([&callCount]() { return ++callCount; }, true);

		token.value();
		int val = token.value().toInt();

		REQUIRE(callCount == 1);
		REQUIRE(val == 1);
	}

	SECTION("LazyWithoutCaching")
	{
		int callCount = 0;
		Token token([&callCount]() { return ++callCount; }, false);

		token.value();
		int val = token.value().toInt();

		REQUIRE(callCount == 2);
		REQUIRE(val == 2);
	}

	SECTION("Compare")
	{
		REQUIRE(Token(13) == Token(13));
		REQUIRE(Token(13) != Token(17));

		REQUIRE(Token("test") == Token("test"));
		REQUIRE(Token("test") != Token("not_test"));

		REQUIRE(Token(QStringList() << "1" << "2") == Token(QStringList() << "1" << "2"));
		REQUIRE(Token(QStringList() << "1" << "2") != Token(QStringList() << "1" << "2" << "3"));
	}
}
