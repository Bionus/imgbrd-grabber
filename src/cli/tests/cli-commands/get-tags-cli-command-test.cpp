#include "catch.h"
#include "cli-commands/get-tags-cli-command.h"


TEST_CASE("GetTagsCliCommand")
{
	SECTION("Validate")
	{
		// Basic usage
		REQUIRE(GetTagsCliCommand(nullptr, nullptr, { nullptr }, 1, 20, 20, 10).validate() == true);

		// Require at least one site
		REQUIRE(GetTagsCliCommand(nullptr, nullptr, {}, 1, 20, 20, 10).validate() == false);
		REQUIRE(GetTagsCliCommand(nullptr, nullptr, { nullptr, nullptr }, 1, 20, 20, 10).validate() == true);
	}
}
