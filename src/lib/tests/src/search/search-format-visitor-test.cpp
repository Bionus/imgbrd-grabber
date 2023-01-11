#include "search/ast/search-node-op.h"
#include "search/ast/search-node-tag.h"
#include "search/search-format.h"
#include "search/search-format-visitor.h"
#include "catch.h"


TEST_CASE("SearchFormatVisitor")
{
	SECTION("OrOnly")
	{
		auto *search = new SearchNodeOp(
			SearchNodeOp::And,
			new SearchNodeTag(Tag("a")),
			new SearchNodeOp(
				SearchNodeOp::Or,
				new SearchNodeTag(Tag("b")),
				new SearchNodeTag(Tag("c"))
			)
		);

		SearchFormat formatNoParenOr({ " ", "" }, { " || ", "" }, false, SearchFormat::Or);
		SearchFormat formatParenAnd({ " ", "" }, { " || ", "" }, true, SearchFormat::And);
		SearchFormat formatNoParenAnd({ " ", "" }, { " || ", "" }, false, SearchFormat::And);

		REQUIRE(SearchFormatVisitor(formatNoParenOr).run(*search) == QString("a b || c"));
		REQUIRE(SearchFormatVisitor(formatParenAnd).run(*search) == QString("a (b || c)"));

		SearchFormatVisitor errorVisitor(formatNoParenAnd);
		REQUIRE(errorVisitor.run(*search) == QString());
		REQUIRE(errorVisitor.error() == QString("A parenthesis is required but the format does not support it"));

		delete search;
	}

	SECTION("OrAnd")
	{
		auto *search = new SearchNodeOp(
			SearchNodeOp::And,
			new SearchNodeTag(Tag("a")),
			new SearchNodeOp(
				SearchNodeOp::Or,
				new SearchNodeTag(Tag("b")),
				new SearchNodeTag(Tag("c"))
			)
		);

		SearchFormat formatNoParenOr({ " & ", "" }, { " | ", "" }, false, SearchFormat::Or);
		SearchFormat formatParenAnd({ " & ", "" }, { " | ", "" }, true, SearchFormat::And);
		SearchFormat formatNoParenAnd({ " & ", "" }, { " | ", "" }, false, SearchFormat::And);

		REQUIRE(SearchFormatVisitor(formatNoParenOr).run(*search) == QString("a & b | c"));
		REQUIRE(SearchFormatVisitor(formatParenAnd).run(*search) == QString("a & (b | c)"));

		SearchFormatVisitor errorVisitor(formatNoParenAnd);
		REQUIRE(errorVisitor.run(*search) == QString());
		REQUIRE(errorVisitor.error() == QString("A parenthesis is required but the format does not support it"));

		delete search;
	}

	SECTION("Prefix")
	{
		auto *search = new SearchNodeOp(
			SearchNodeOp::And,
			new SearchNodeTag(Tag("a")),
			new SearchNodeOp(
				SearchNodeOp::Or,
				new SearchNodeTag(Tag("b")),
				new SearchNodeTag(Tag("c"))
			)
		);

		SearchFormat formatNoParenOr({ " ", "" }, { " ", "~" }, false, SearchFormat::Or);
		SearchFormat formatParenAnd({ " ", "" }, { " ", "~" }, true, SearchFormat::And);
		SearchFormat formatNoParenAnd({ " ", "" }, { " ", "~" }, false, SearchFormat::And);

		REQUIRE(SearchFormatVisitor(formatNoParenOr).run(*search) == QString("a ~b ~c"));
		REQUIRE(SearchFormatVisitor(formatParenAnd).run(*search) == QString("a (~b ~c)"));

		SearchFormatVisitor errorVisitor(formatNoParenAnd);
		REQUIRE(errorVisitor.run(*search) == QString());
		REQUIRE(errorVisitor.error() == QString("A parenthesis is required but the format does not support it"));

		delete search;
	}
}
