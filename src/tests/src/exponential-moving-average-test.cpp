#include "exponential-moving-average.h"
#include "catch.h"


TEST_CASE("ExponentialMovingAverage")
{
	SECTION("Empty")
	{
		ExponentialMovingAverage avg(0.5);

		REQUIRE(avg.average() == 0.0);
	}

	SECTION("Clear")
	{
		ExponentialMovingAverage avg(0.5);
		avg.addValue(1);
		avg.clear();

		REQUIRE(avg.average() == 0.0);
	}

	SECTION("FirstValue")
	{
		ExponentialMovingAverage avg(0.5);
		avg.addValue(1);

		REQUIRE(avg.average() == 1.0);
	}

	SECTION("Basic")
	{
		ExponentialMovingAverage avg(0.5);
		avg.addValue(2);
		avg.addValue(4);
		avg.addValue(5);

		REQUIRE(avg.average() == 4.0);
	}

	SECTION("SetSmoothingFactor")
	{
		ExponentialMovingAverage avg(1);
		avg.setSmoothingFactor(0.5);
		avg.addValue(2);
		avg.addValue(4);
		avg.addValue(5);

		REQUIRE(avg.average() == 4.0);
	}
}
