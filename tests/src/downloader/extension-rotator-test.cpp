#include "downloader/extension-rotator.h"
#include "catch.h"


TEST_CASE("ExtensionRotatorTest")
{
	SECTION("Basic")
	{
		ExtensionRotator rotator("jpg", QStringList() << "jpg" << "png" << "gif");

		REQUIRE(rotator.next() == QString("png"));
		REQUIRE(rotator.next() == QString("gif"));
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Loop")
	{
		ExtensionRotator rotator("png", QStringList() << "jpg" << "png" << "gif");

		REQUIRE(rotator.next() == QString("gif"));
		REQUIRE(rotator.next() == QString("jpg"));
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Not found")
	{
		ExtensionRotator rotator("mp4", QStringList() << "jpg" << "png" << "gif");

		REQUIRE(rotator.next() == QString("jpg"));
		REQUIRE(rotator.next() == QString("png"));
		REQUIRE(rotator.next() == QString("gif"));
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Keep empty")
	{
		ExtensionRotator rotator("jpg", QStringList() << "jpg" << "png" << "gif");

		REQUIRE(rotator.next() == QString("png"));
		REQUIRE(rotator.next() == QString("gif"));
		REQUIRE(rotator.next() == QString());
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Empty first")
	{
		ExtensionRotator rotator("", QStringList() << "jpg" << "png" << "gif");

		REQUIRE(rotator.next() == QString("jpg"));
		REQUIRE(rotator.next() == QString("png"));
		REQUIRE(rotator.next() == QString("gif"));
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Empty list")
	{
		ExtensionRotator rotator("jpg", QStringList());

		REQUIRE(rotator.next() == QString());
	}

	SECTION("Empty both")
	{
		ExtensionRotator rotator("", QStringList());

		REQUIRE(rotator.next() == QString());
	}

	SECTION("Copy constructor")
	{
		ExtensionRotator rotator("mp4", QStringList() << "jpg" << "png" << "gif");
		REQUIRE(rotator.next() == QString("jpg"));
		REQUIRE(rotator.next() == QString("png"));

		ExtensionRotator cpy(rotator);
		REQUIRE(cpy.next() == QString("gif"));
		REQUIRE(cpy.next() == QString());
	}
}
