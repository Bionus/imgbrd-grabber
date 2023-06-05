#include <QSize>
#include "catch.h"
#include "utils/size-utils.h"


TEST_CASE("Size utils")
{
	SECTION("isBigger")
	{
		SECTION("Basic")
		{
			REQUIRE(isBigger(QSize(300, 300), QSize(150, 150)));
			REQUIRE(!isBigger(QSize(150, 150), QSize(300, 300)));
		}

		SECTION("Only width")
		{
			REQUIRE(isBigger(QSize(300, -1), QSize(150, -1)));
			REQUIRE(!isBigger(QSize(150, -1), QSize(300, -1)));
		}

		SECTION("Only height")
		{
			REQUIRE(isBigger(QSize(-1, 300), QSize(-1, 150)));
			REQUIRE(!isBigger(QSize(-1, 150), QSize(-1, 300)));
		}

		SECTION("Different ratios")
		{
			REQUIRE(!isBigger(QSize(300, 150), QSize(150, 300)));
			REQUIRE(!isBigger(QSize(150, 300), QSize(300, 150)));
		}

		SECTION("Different dimensions")
		{
			REQUIRE(!isBigger(QSize(150, -1), QSize(-1, 150)));
			REQUIRE(!isBigger(QSize(-1, 150), QSize(150, -1)));
		}

		SECTION("Invalid")
		{
			REQUIRE(isBigger(QSize(150, 150), QSize(-1, -1)));
			REQUIRE(!isBigger(QSize(-1, -1), QSize(150, 150)));
		}
	}

	SECTION("isInRange")
	{
		QSize size(234, 234);
		REQUIRE(!isInRange(size, 0, 150));
		REQUIRE(isInRange(size, 150, 300));
		REQUIRE(isInRange(size, 0, 500));
		REQUIRE(!isInRange(size, 300, 1500));
	}
}
