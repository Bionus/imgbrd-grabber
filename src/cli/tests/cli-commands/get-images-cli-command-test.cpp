#include "catch.h"
#include "cli.h"
#include "cli-commands/get-images-cli-command.h"


TEST_CASE("GetImagesCliCommand")
{
	SECTION("Validate")
	{
		// Basic usage
		REQUIRE(GetImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 1, 20, "", "", 20, true, false, false, {}).validate() == true);
		REQUIRE(GetImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 1, 20, "", "", -1, true, false, false, {}).validate() == true);

		// Require at least one site
		REQUIRE(GetImagesCliCommand(nullptr, nullptr, { "search" }, {}, {}, 1, 20, "", "", 20, true, false, false, {}).validate() == false);
		REQUIRE(GetImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr, nullptr }, 1, 20, "", "", 20, true, false, false, {}).validate() == true);

		// Require a starting page more than 0
		REQUIRE(GetImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 0, 20, "", "", 20, true, false, false, {}).validate() == false);

		// Require a number of images per page more than 0
		REQUIRE(GetImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 1, 0, "", "", 20, true, false, false, {}).validate() == false);

		// Require a valid image limit
		REQUIRE(GetImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 1, 20, "", "", 0, true, false, false, {}).validate() == false);
	}
}
