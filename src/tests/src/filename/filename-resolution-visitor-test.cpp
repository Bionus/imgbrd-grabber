#include <QSet>
#include <QString>
#include "filename/filename-parser.h"
#include "filename/filename-resolution-visitor.h"
#include "catch.h"


TEST_CASE("FilenameResolutionVisitor")
{
	SECTION("Empty")
	{
		FilenameParser parser("");
		auto ast = parser.parseRoot();

		FilenameResolutionVisitor resolutionVisitor;
		auto results = resolutionVisitor.run(*ast);

		REQUIRE(results == QSet<QString>());
	}

	SECTION("Basic")
	{
		FilenameParser parser("out/%md5:opt%.%ext%");
		auto ast = parser.parseRoot();

		FilenameResolutionVisitor resolutionVisitor;
		auto results = resolutionVisitor.run(*ast);

		REQUIRE(results == QSet<QString>() << "md5" << "ext");
	}

	SECTION("Conditional")
	{
		FilenameParser parser("out/<%id%?some tag is present:%rating%>/%md5%.%ext%");
		auto ast = parser.parseRoot();

		FilenameResolutionVisitor resolutionVisitor;
		auto results = resolutionVisitor.run(*ast);

		REQUIRE(results == QSet<QString>() << "id" << "rating" << "md5" << "ext");
	}

	SECTION("Duplicates")
	{
		FilenameParser parser("%md5%/file-%md5:opt%.%ext%");
		auto ast = parser.parseRoot();

		FilenameResolutionVisitor resolutionVisitor;
		auto results = resolutionVisitor.run(*ast);

		REQUIRE(results == QSet<QString>() << "md5" << "ext");
	}
}
