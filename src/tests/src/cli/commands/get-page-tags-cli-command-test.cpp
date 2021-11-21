#include "catch.h"
#include "cli/commands/get-page-tags-cli-command.h"


TEST_CASE("GetPageTagsCliCommand")
{
	SECTION("Validate")
	{
		// Basic usage
		REQUIRE(GetPageTagsCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 1, 20, 10).validate() == true);

		// Require at least one site
		REQUIRE(GetPageTagsCliCommand(nullptr, nullptr, { "search" }, {}, {}, 1, 20, 10).validate() == false);
		REQUIRE(GetPageTagsCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr, nullptr }, 1, 20, 10).validate() == true);
	}
}
