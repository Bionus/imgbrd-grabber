#include <QLabel>
#include "about-window.h"
#include "catch.h"


TEST_CASE("AboutWindow")
{
	SECTION("Show the current application version")
	{
		const auto *window = new AboutWindow("1.2.3", nullptr);
		const auto *labelCurrent = window->findChild<QLabel*>("labelCurrent");

		REQUIRE(labelCurrent->text().startsWith("1.2.3"));
	}
}
