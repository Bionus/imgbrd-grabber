#include "models/pool.h"
#include "catch.h"


TEST_CASE("Pool")
{
	SECTION("GetId")
	{
		Pool pool(123, "Test pool", 1, 2, 3);
		REQUIRE(pool.id() == 123);
	}

	SECTION("GetName")
	{
		Pool pool(123, "Test pool", 1, 2, 3);
		REQUIRE(pool.name() == QString("Test pool"));
	}

	SECTION("GetCurrent")
	{
		Pool pool(123, "Test pool", 1, 2, 3);
		REQUIRE(pool.current() == 1);
	}

	SECTION("GetNext")
	{
		Pool pool(123, "Test pool", 1, 2, 3);
		REQUIRE(pool.next() == 2);
	}

	SECTION("GetPrevious")
	{
		Pool pool(123, "Test pool", 1, 2, 3);
		REQUIRE(pool.previous() == 3);
	}
}
