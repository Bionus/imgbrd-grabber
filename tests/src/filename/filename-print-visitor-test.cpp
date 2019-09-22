#include <QString>
#include "filename/filename-parser.h"
#include "filename/filename-print-visitor.h"
#include "catch.h"


TEST_CASE("FilenamePrintVisitor")
{
	SECTION("Empty")
	{
		FilenameParser parser("");
		auto ast = parser.parseRoot();

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);

		REQUIRE(result == QString("Root()"));
	}

	SECTION("Basic")
	{
		FilenameParser parser("out/%md5:opt%.%ext%");
		auto ast = parser.parseRoot();

		FilenamePrintVisitor printVisitor;
		QString result = printVisitor.run(*ast);

		REQUIRE(result == QString("Root(Text('out/');Variable('md5';opt);Text('.');Variable('ext'))"));
	}
}
