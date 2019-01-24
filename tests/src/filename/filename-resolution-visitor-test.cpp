#include "filename-resolution-visitor-test.h"
#include <QSet>
#include <QString>
#include <QtTest>
#include "filename/filename-parser.h"
#include "filename/filename-resolution-visitor.h"


void FilenameResolutionVisitorTest::testEmpty()
{
	FilenameParser parser("");
	auto ast = parser.parseRoot();

	FilenameResolutionVisitor resolutionVisitor;
	auto results = resolutionVisitor.run(*ast);

	QCOMPARE(results, QSet<QString>());
}

void FilenameResolutionVisitorTest::testBasic()
{
	FilenameParser parser("out/%md5:opt%.%ext%");
	auto ast = parser.parseRoot();

	FilenameResolutionVisitor resolutionVisitor;
	auto results = resolutionVisitor.run(*ast);

	QCOMPARE(results, QSet<QString>() << "md5" << "ext");
}

void FilenameResolutionVisitorTest::testConditional()
{
	FilenameParser parser("out/<%id%?some tag is present:%rating%>/%md5%.%ext%");
	auto ast = parser.parseRoot();

	FilenameResolutionVisitor resolutionVisitor;
	auto results = resolutionVisitor.run(*ast);

	QCOMPARE(results, QSet<QString>() << "id" << "rating" << "md5" << "ext");
}

void FilenameResolutionVisitorTest::testDuplicates()
{
	FilenameParser parser("%md5%/file-%md5:opt%.%ext%");
	auto ast = parser.parseRoot();

	FilenameResolutionVisitor resolutionVisitor;
	auto results = resolutionVisitor.run(*ast);

	QCOMPARE(results, QSet<QString>() << "md5" << "ext");
}


QTEST_MAIN(FilenameResolutionVisitorTest)
