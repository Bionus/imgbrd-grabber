#include "catch.h"
#include "utils/logging.h"


TEST_CASE("Logging")
{
	SECTION("logToHtml")
	{
		SECTION("Basic usage")
		{
			REQUIRE(logToHtml("") == QString());
		}

		SECTION("Coloring")
		{
			REQUIRE(logToHtml("[12:34:56.123][Warning] test") == QString("<span style='color:darkgreen'>[12:34:56.123]</span><span style='color:orange'>[Warning] test</span>"));
		}

		SECTION("Escape HTML")
		{
			REQUIRE(logToHtml("<escape>") == QString("&lt;escape&gt;"));
			REQUIRE(logToHtml("test <escape>") == QString("test &lt;escape&gt;"));
			REQUIRE(logToHtml("<escape> test") == QString("&lt;escape&gt; test"));
		}

		SECTION("Links")
		{
			REQUIRE(logToHtml("`https://test.com`") == QString("<a href=\"https://test.com\">https://test.com</a>"));
			REQUIRE(logToHtml("test `https://test.com`") == QString("test <a href=\"https://test.com\">https://test.com</a>"));
			REQUIRE(logToHtml("`https://test.com` test") == QString("<a href=\"https://test.com\">https://test.com</a> test"));
		}
	}
}
