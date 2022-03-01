#include <QMap>
#include <QString>
#include "catch.h"
#include "filename/conditional-filename.h"
#include "loader/token.h"


TEST_CASE("ConditionalFilename")
{
	SECTION("Empty condition")
	{
		ConditionalFilename filename("", "yes" ,  "/");

		REQUIRE(!filename.matches({}, nullptr));
	}

	SECTION("Parse error")
	{
		ConditionalFilename filename("%test", "yes" ,  "/");

		REQUIRE(!filename.matches({}, nullptr));
	}

	SECTION("Basic usage")
	{
		ConditionalFilename filename("%test%", "yes" ,  "/");

		REQUIRE(!filename.matches({{ "test", Token("") }}, nullptr));
		REQUIRE(filename.matches({{ "test", Token("yes") }}, nullptr));
	}
}
