#include "updater/program-updater.h"
#include "catch.h"


TEST_CASE("Updater")
{
	ProgramUpdater updater;
	
	SECTION("CompareEqual")
	{
		REQUIRE(updater.compareVersions("1.0.0", "1.0.0") == 0);
		REQUIRE(updater.compareVersions("1.4.0", "1.4.0") == 0);
		REQUIRE(updater.compareVersions("1.4.7", "1.4.7") == 0);
	}

	SECTION("CompareEqualAlphas")
	{
		REQUIRE(updater.compareVersions("1.0.0a2", "1.0.0a2") == 0);
		REQUIRE(updater.compareVersions("1.4.0a2", "1.4.0a2") == 0);
		REQUIRE(updater.compareVersions("1.4.7a2", "1.4.7a2") == 0);
	}


	SECTION("CompareMinor")
	{
		REQUIRE(updater.compareVersions("1.0.1", "1.0.0") == 1);
		REQUIRE(updater.compareVersions("1.0.0", "1.0.1") == -1);
	}

	SECTION("CompareNormal")
	{
		REQUIRE(updater.compareVersions("1.1.0", "1.0.0") == 1);
		REQUIRE(updater.compareVersions("1.0.0", "1.1.0") == -1);
	}

	SECTION("CompareMajor")
	{
		REQUIRE(updater.compareVersions("2.0.0", "1.0.0") == 1);
		REQUIRE(updater.compareVersions("1.0.0", "2.0.0") == -1);
	}

	SECTION("CompareTen")
	{
		REQUIRE(updater.compareVersions("2.0.0", "1.10.0") == 1);
		REQUIRE(updater.compareVersions("1.10.0", "2.0.0") == -1);
	}

	SECTION("CompareMissing")
	{
		REQUIRE(updater.compareVersions("1.0.1", "1.0") == 1);
		REQUIRE(updater.compareVersions("1.0", "1.0.1") == -1);
	}


	SECTION("CompareAlphas")
	{
		REQUIRE(updater.compareVersions("1.0.0a3", "1.0.0a2") == 1);
		REQUIRE(updater.compareVersions("1.0.0a2", "1.0.0a3") == -1);
	}

	SECTION("CompareAlphaToNew")
	{
		REQUIRE(updater.compareVersions("1.0.0", "1.0.0a3") == 1);
		REQUIRE(updater.compareVersions("1.0.0a3", "1.0.0") == -1);
	}

	SECTION("CompareAlphaToOld")
	{
		REQUIRE(updater.compareVersions("1.0.0a3", "0.1.0") == 1);
		REQUIRE(updater.compareVersions("0.1.0", "1.0.0a3") == -1);
	}

	SECTION("CompareAlphaToBeta")
	{
		REQUIRE(updater.compareVersions("1.0.0b1", "1.0.0a3") == 1);
		REQUIRE(updater.compareVersions("1.0.0a3", "1.0.0b1") == -1);
	}
}
