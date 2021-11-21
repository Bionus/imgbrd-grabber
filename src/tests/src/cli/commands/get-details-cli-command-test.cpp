#include "catch.h"
#include "cli/commands/get-details-cli-command.h"


TEST_CASE("GetDetailsCliCommand")
{
	SECTION("Validate")
	{
		// Basic usage
		REQUIRE(GetDetailsCliCommand(nullptr, nullptr, { nullptr }, "pageUrl").validate() == true);

		// Require a page URL
		REQUIRE(GetDetailsCliCommand(nullptr, nullptr, { nullptr }, "").validate() == false);

		// Require exactly one site
		REQUIRE(GetDetailsCliCommand(nullptr, nullptr, {}, "pageUrl").validate() == false);
		REQUIRE(GetDetailsCliCommand(nullptr, nullptr, { nullptr, nullptr }, "pageUrl").validate() == false);
	}
}
