#include <QScopedPointer>
#include "catch.h"
#include "utils/html-node.h"


TEST_CASE("HTML parsing utils")
{
	SECTION("Basic HTML")
	{
		QScopedPointer<HtmlNode> node(HtmlNode::fromString("<p><span>Hello</span> world!</p>", true));

		REQUIRE(node != nullptr);
		REQUIRE(node->outerHTML() == QString("<p><span>Hello</span> world!</p>"));
		REQUIRE(node->innerHTML() == QString("<span>Hello</span> world!"));
		REQUIRE(node->innerText() == QString("Hello world!"));
	}

	SECTION("HTML attributes")
	{
		QScopedPointer<HtmlNode> node(HtmlNode::fromString("<p key1=\"val1\" key2='val2' key3=val3 />", true));

		REQUIRE(node != nullptr);
		REQUIRE(node->attr("key1") == QString("val1"));
		REQUIRE(node->attr("key2") == QString("val2"));
		REQUIRE(node->attr("key3") == QString("val3"));
		REQUIRE(node->attr("key4") == QString());
	}
}
