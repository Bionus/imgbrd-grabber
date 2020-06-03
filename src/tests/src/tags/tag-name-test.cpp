#include "tags/tag-name.h"
#include "catch.h"


TEST_CASE("TagName")
{
	SECTION("NormalizedValid")
	{
		TagNameFormat capsSpace(TagNameFormat::Caps, " ");
		TagNameFormat upperFirstDash(TagNameFormat::UpperFirst, "-");

		REQUIRE(TagName("tag_name").normalized() == QString("tag_name"));
		REQUIRE(TagName("TAG NAME", capsSpace).normalized() == QString("tag_name"));
		REQUIRE(TagName("Tag-name", upperFirstDash).normalized() == QString("tag_name"));
	}

	SECTION("NormalizedInvalid")
	{
		REQUIRE(TagName("TAG NAME").normalized() == QString("TAG NAME"));
	}

	SECTION("Formatted")
	{
		TagNameFormat capsSpace(TagNameFormat::Caps, " ");
		TagNameFormat upperFirstDash(TagNameFormat::UpperFirst, "-");

		REQUIRE(TagName("tag_name").formatted(capsSpace) == QString("TAG NAME"));
		REQUIRE(TagName("tag_name").formatted(upperFirstDash) == QString("Tag-name"));
		REQUIRE(TagName("Tag-name", upperFirstDash).formatted(capsSpace) == QString("TAG NAME"));
		REQUIRE(TagName("TAG NAME", capsSpace).formatted(upperFirstDash) == QString("Tag-name"));
	}

	SECTION("Compare")
	{
		TagNameFormat capsSpace(TagNameFormat::Caps, " ");
		TagNameFormat upperFirstDash(TagNameFormat::UpperFirst, "-");

		// Valid
		REQUIRE(TagName("Tag-name", upperFirstDash) == TagName("tag_name"));
		REQUIRE(TagName("TAG NAME", capsSpace) == TagName("tag_name"));
		REQUIRE(TagName("Tag-name", upperFirstDash) == TagName("TAG NAME", capsSpace));

		// Invalid
		REQUIRE(TagName("Tag-name-2", upperFirstDash) != TagName("tag_name"));
		REQUIRE(TagName("TAG NAME 2", capsSpace) != TagName("tag_name"));
		REQUIRE(TagName("Tag-name 2", upperFirstDash) != TagName("TAG NAME", capsSpace));
	}
}
