#include "catch.h"
#include "cli/commands/get-page-count-cli-command.h"


TEST_CASE("GetPageCountCliCommand")
{
	SECTION("Validate")
	{
		// Basic usage
		REQUIRE(GetPageCountCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 1, 20).validate() == true);

		// Require at least one site
		REQUIRE(GetPageCountCliCommand(nullptr, nullptr, { "search" }, {}, {}, 1, 20).validate() == false);
		REQUIRE(GetPageCountCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr, nullptr }, 1, 20).validate() == true);
	}
}
