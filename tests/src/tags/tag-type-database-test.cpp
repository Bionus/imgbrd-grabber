#include "tags/tag-type-database.h"
#include "catch.h"


TEST_CASE("TagTypeDatabase")
{
	TagTypeDatabase database("tests/resources/tag-types.txt");
	database.load();

	SECTION("Basic load")
	{
		QMap<int, TagType> types = database.getAll();
		REQUIRE(types.count() == 4);
		REQUIRE(types.keys() == QList<int>() << 0 << 1 << 3 << 4);
		REQUIRE(types.value(0).name() == "general");
		REQUIRE(types.value(1).name() == "artist");
		REQUIRE(types.value(3).name() == "copyright");
		REQUIRE(types.value(4).name() == "character");
	}

	SECTION("Double load")
	{
		REQUIRE(database.load());
		REQUIRE(database.getAll().count() == 4);
	}

	SECTION("Get by ID")
	{
		REQUIRE(database.contains(3));
		REQUIRE(database.get(3).name() == "copyright");

		REQUIRE(!database.contains(2));
	}

	SECTION("Get by name")
	{
		REQUIRE(database.get(TagType("copyright")) == 3);
		REQUIRE(database.get(TagType("not_found")) == -1);
		REQUIRE(database.get(TagType("not_found"), true) == 5);
	}
}
