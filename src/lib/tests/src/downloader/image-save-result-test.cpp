#include "downloader/image-save-result.h"
#include "catch.h"


TEST_CASE("ImageSaveResultTest")
{
	SECTION("Equality operator")
	{
		ImageSaveResult a;
		a.path = "path";
		a.size = Image::Size::Full;
		a.result = Image::SaveResult::Saved;

		ImageSaveResult b;
		b.path = "path";
		b.size = Image::Size::Full;
		b.result = Image::SaveResult::Saved;

		ImageSaveResult c;
		c.path = "sample";
		c.size = Image::Size::Sample;
		c.result = Image::SaveResult::Saved;

		REQUIRE(a == b);
		REQUIRE(b == a);
		REQUIRE(a != c);
		REQUIRE(b != c);
		REQUIRE(c == c);
	}
}
