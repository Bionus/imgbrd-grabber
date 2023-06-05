#include <QString>
#include "catch.h"
#include "utils/read-write-path.h"


TEST_CASE("Read and write path")
{
	SECTION("readWritePath")
	{
		ReadWritePath parent("tests/resources/read", "tests/resources/write");
		ReadWritePath child = parent.readWritePath("child");

		REQUIRE(child.readPath() == QString("tests/resources/read/child"));
		REQUIRE(child.writePath() == QString("tests/resources/write/child"));
	}
}
