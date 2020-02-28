#include "tags/tag-name-format.h"
#include "catch.h"


TEST_CASE("TagNameFormat")
{
	SECTION("Lower")
	{
		TagNameFormat format(TagNameFormat::Lower, "_");

		REQUIRE(format.formatted(QStringList()) == QString(""));
		REQUIRE(format.formatted(QStringList() << "test") == QString("test"));
		REQUIRE(format.formatted(QStringList() << "test" << "tag") == QString("test_tag"));
		REQUIRE(format.formatted(QStringList() << "Test" << "tAG") == QString("test_tag"));
	}

	SECTION("UpperFirst")
	{
		TagNameFormat format(TagNameFormat::UpperFirst, "_");

		REQUIRE(format.formatted(QStringList()) == QString(""));
		REQUIRE(format.formatted(QStringList() << "test") == QString("Test"));
		REQUIRE(format.formatted(QStringList() << "test" << "tag") == QString("Test_tag"));
		REQUIRE(format.formatted(QStringList() << "Test" << "tAG") == QString("Test_tag"));
	}

	SECTION("Upper")
	{
		TagNameFormat format(TagNameFormat::Upper, "_");

		REQUIRE(format.formatted(QStringList()) == QString(""));
		REQUIRE(format.formatted(QStringList() << "test") == QString("Test"));
		REQUIRE(format.formatted(QStringList() << "test" << "tag") == QString("Test_Tag"));
		REQUIRE(format.formatted(QStringList() << "Test" << "tAG") == QString("Test_Tag"));
	}

	SECTION("Caps")
	{
		TagNameFormat format(TagNameFormat::Caps, "_");

		REQUIRE(format.formatted(QStringList()) == QString(""));
		REQUIRE(format.formatted(QStringList() << "test") == QString("TEST"));
		REQUIRE(format.formatted(QStringList() << "test" << "tag") == QString("TEST_TAG"));
		REQUIRE(format.formatted(QStringList() << "Test" << "tAG") == QString("TEST_TAG"));
	}

	SECTION("Unknown")
	{
		TagNameFormat format((TagNameFormat::CaseFormat)123, " ");

		REQUIRE(format.formatted(QStringList()) == QString(""));
		REQUIRE(format.formatted(QStringList() << "test") == QString("test"));
		REQUIRE(format.formatted(QStringList() << "test" << "tag") == QString("test tag"));
		REQUIRE(format.formatted(QStringList() << "Test" << "tAG") == QString("Test tAG"));
	}
}
