#include "search-format-visitor-test.h"
#include <QtTest>
#include "search/ast/search-node-op.h"
#include "search/ast/search-node-tag.h"
#include "search/search-format.h"
#include "search/search-format-visitor.h"


void SearchFormatVisitorTest::testOrOnly()
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

	QCOMPARE(SearchFormatVisitor(formatNoParenOr).run(*search), QString("a b || c"));
	QCOMPARE(SearchFormatVisitor(formatParenAnd).run(*search), QString("a (b || c)"));

	SearchFormatVisitor errorVisitor(formatNoParenAnd);
	QCOMPARE(errorVisitor.run(*search), QString());
	QCOMPARE(errorVisitor.error(), QString("A parenthesis is required but the format does not support it"));

	delete search;
}

void SearchFormatVisitorTest::testOrAnd()
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

	QCOMPARE(SearchFormatVisitor(formatNoParenOr).run(*search), QString("a & b | c"));
	QCOMPARE(SearchFormatVisitor(formatParenAnd).run(*search), QString("a & (b | c)"));

	SearchFormatVisitor errorVisitor(formatNoParenAnd);
	QCOMPARE(errorVisitor.run(*search), QString());
	QCOMPARE(errorVisitor.error(), QString("A parenthesis is required but the format does not support it"));

	delete search;
}

void SearchFormatVisitorTest::testPrefix()
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

	QCOMPARE(SearchFormatVisitor(formatNoParenOr).run(*search), QString("a ~b ~c"));
	QCOMPARE(SearchFormatVisitor(formatParenAnd).run(*search), QString("a (~b ~c)"));

	SearchFormatVisitor errorVisitor(formatNoParenAnd);
	QCOMPARE(errorVisitor.run(*search), QString());
	QCOMPARE(errorVisitor.error(), QString("A parenthesis is required but the format does not support it"));

	delete search;
}


QTEST_MAIN(SearchFormatVisitorTest)
