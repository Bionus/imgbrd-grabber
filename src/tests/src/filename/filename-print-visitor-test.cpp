#include <QString>
#include "filename/ast/filename-node-condition.h"
#include "filename/ast/filename-node-root.h"
#include "filename/filename-parser.h"
#include "filename/filename-print-visitor.h"
#include "catch.h"


TEST_CASE("FilenamePrintVisitor")
{
	SECTION("Empty")
	{
		FilenameParser parser("");
		auto *ast = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root()"));
	}

	SECTION("Basic")
	{
		FilenameParser parser("out/%md5:opt%.%ext%");
		auto *ast = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(Text('out/');Variable('md5';opt);Text('.');Variable('ext'))"));
	}

	SECTION("Conditional")
	{
		FilenameParser parser("<\"tag\"&!%token%?true:false>");
		auto *ast = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(Conditional(ConditionOp(ConditionTag('tag');And;ConditionInvert(ConditionToken('token')));Text('true');Text('false')))"));
	}

	SECTION("Conditional with many operands")
	{
		FilenameParser parser(R"(<a&b|c&d|e&f?true:false>)");
		auto *ast = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(Conditional(ConditionOp(ConditionOp(ConditionOp(ConditionTag('a');And;ConditionTag('b'));Or;ConditionOp(ConditionTag('c');And;ConditionTag('d')));Or;ConditionOp(ConditionTag('e');And;ConditionTag('f')));Text('true');Text('false')))"));
	}

	SECTION("Conditional with many operands and parentheses with same priority")
	{
		FilenameParser parser(R"(<(a&b)|(c&d)|(e&f)?true:false>)");
		auto *ast = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(Conditional(ConditionOp(ConditionOp(ConditionOp(ConditionTag('a');And;ConditionTag('b'));Or;ConditionOp(ConditionTag('c');And;ConditionTag('d')));Or;ConditionOp(ConditionTag('e');And;ConditionTag('f')));Text('true');Text('false')))"));
	}

	SECTION("Conditional with many operands and parentheses with different priority")
	{
		FilenameParser parser(R"(<a&(b|c)&(d|e)&f?true:false>)");
		auto *ast = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(Conditional(ConditionOp(ConditionOp(ConditionOp(ConditionTag('a');And;ConditionOp(ConditionTag('b');Or;ConditionTag('c')));And;ConditionOp(ConditionTag('d');Or;ConditionTag('e')));And;ConditionTag('f'));Text('true');Text('false')))"));
	}

	SECTION("Conditional (legacy)")
	{
		FilenameParser parser("<legacy conditional \"tag\" -%token%>");
		auto *ast = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(Conditional(ConditionOp(ConditionTag('tag');And;ConditionIgnore(ConditionToken('token')));Root(Text('legacy conditional ');ConditionTag('tag');Text(' ');ConditionIgnore(ConditionToken('token')))))"));
	}

	SECTION("JavaScript")
	{
		FilenameParser parser("javascript:md5 + '.' + ext");
		auto *ast = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(JavaScript(md5 + '.' + ext))"));
	}

	SECTION("JavaScript conditional")
	{
		FilenameParser parser("javascript:artist || copyright");
		auto *ast = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("ConditionJavaScript(artist || copyright)"));
	}
}
