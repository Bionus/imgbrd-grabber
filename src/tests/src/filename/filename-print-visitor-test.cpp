#include <QString>
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

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root()"));
	}

	SECTION("Basic")
	{
		FilenameParser parser("out/%md5:opt%.%ext%");
		auto *ast = parser.parseRoot();

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(Text('out/');Variable('md5';opt);Text('.');Variable('ext'))"));
	}

	SECTION("Conditional")
	{
		FilenameParser parser("<\"tag\"&!%token%?true:false>");
		auto *ast = parser.parseRoot();

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(Conditional(ConditionOp(ConditionTag('tag');And;ConditionInvert(ConditionToken('token')));Text('true');Text('false')))"));
	}

	SECTION("Conditional (legacy)")
	{
		FilenameParser parser("<legacy conditional \"tag\" -%token%>");
		auto *ast = parser.parseRoot();

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(Conditional(ConditionOp(ConditionTag('tag');And;ConditionIgnore(ConditionToken('token')));Root(Text('legacy conditional ');ConditionTag('tag');Text(' ');ConditionIgnore(ConditionToken('token')))))"));
	}

	SECTION("JavaScript")
	{
		FilenameParser parser("javascript:md5 + '.' + ext");
		auto *ast = parser.parseRoot();

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);
		delete ast;

		REQUIRE(result == QString("Root(JavaScript(md5 + '.' + ext))"));
	}
}
