#include "catch.h"
#include "cli-commands/download-images-cli-command.h"


TEST_CASE("DownloadImagesCliCommand")
{
	SECTION("Validate")
	{
		// Basic usage
		REQUIRE(DownloadImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 1, 20, "%md5%.%ext%", ".", 20, true, false, false, {}).validate() == true);

		// Require at least one site
		REQUIRE(DownloadImagesCliCommand(nullptr, nullptr, { "search" }, {}, {}, 1, 20, "%md5%.%ext%", ".", 20, true, false, false, {}).validate() == false);
		REQUIRE(DownloadImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr, nullptr }, 1, 20, "%md5%.%ext%", ".", 20, true, false, false, {}).validate() == true);

		// Require a number of images per page more than 0
		REQUIRE(DownloadImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 1, 0, "%md5%.%ext%", ".", 20, true, false, false, {}).validate() == false);

		// Require a max of more than 0
		REQUIRE(DownloadImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 1, 20, "%md5%.%ext%", ".", 0, true, false, false, {}).validate() == false);

		// Require a non-empty filename
		REQUIRE(DownloadImagesCliCommand(nullptr, nullptr, { "search" }, {}, { nullptr }, 1, 20, "", ".", 20, true, false, false, {}).validate() == false);
	}
}
