#include "catch.h"
#include "cli-commands/load-tag-database-cli-command.h"


TEST_CASE("LoadTagDatabaseCliCommand")
{
	SECTION("Validate")
	{
		// Basic usage
		REQUIRE(LoadTagDatabaseCliCommand(nullptr, { nullptr }, 100).validate() == true);

		// Require at least one site
		REQUIRE(LoadTagDatabaseCliCommand(nullptr, {}, 100).validate() == false);
		REQUIRE(LoadTagDatabaseCliCommand(nullptr, { nullptr, nullptr }, 100).validate() == true);

		// Allow less than 100 for min tag count
		REQUIRE(LoadTagDatabaseCliCommand(nullptr, { nullptr }, 20).validate() == true);
	}
}
