#include "search/ast/search-node-op.h"
#include "search/ast/search-node-tag.h"
#include "search/search-parser.h"
#include "search/search-format-visitor.h"
#include "catch.h"


TEST_CASE("SearchParser")
{
	SearchFormat searchFormat({ " & ", "" }, { " | ", "" }, true, SearchFormat::And);
	TagNameFormat tagNameFormat(TagNameFormat::CaseFormat::Upper, "-");

	SECTION("Empty search")
	{
		QScopedPointer<SearchNode> ast(SearchParser("").parse());

		REQUIRE(ast.data() == nullptr);
	}

	SECTION("Single tag")
	{
		QScopedPointer<SearchNode> ast(SearchParser("test_tag").parse());

		auto *tagNode = dynamic_cast<SearchNodeTag*>(ast.data());
		REQUIRE(tagNode != nullptr);
		REQUIRE(tagNode->tag == QStringList() << "test" << "tag");

		REQUIRE(SearchFormatVisitor(searchFormat, tagNameFormat).run(*ast) == QString("Test-Tag"));
	}

	SECTION("Two tags")
	{
		QScopedPointer<SearchNode> ast(SearchParser("first_tag second_tag").parse());

		auto *opNode = dynamic_cast<SearchNodeOp*>(ast.data());
		REQUIRE(opNode != nullptr);
		REQUIRE(opNode->op == SearchNodeOp::Operator::And);

		REQUIRE(SearchFormatVisitor(searchFormat, tagNameFormat).run(*ast) == QString("First-Tag & Second-Tag"));
	}

	SECTION("Three tags")
	{
		QScopedPointer<SearchNode> ast(SearchParser("first_tag second_tag third_tag").parse());

		auto *opNode = dynamic_cast<SearchNodeOp*>(ast.data());
		REQUIRE(opNode != nullptr);
		REQUIRE(opNode->op == SearchNodeOp::Operator::And);

		REQUIRE(SearchFormatVisitor(searchFormat, tagNameFormat).run(*ast) == QString("First-Tag & Second-Tag & Third-Tag"));
	}
}
