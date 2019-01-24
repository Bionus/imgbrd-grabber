#include "filename-parser-test.h"
#include <QMap>
#include <QString>
#include <QtTest>
#include "filename/filename-parser.h"
#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-text.h"
#include "filename/ast/filename-node-variable.h"


void FilenameParserTest::testParseText()
{
	FilenameParser parser("image.png");
	auto filename = parser.parseRoot();

	QCOMPARE(filename->exprs.count(), 1);

	auto txt = dynamic_cast<FilenameNodeText*>(filename->exprs[0]);
	QVERIFY(txt != nullptr);
	QCOMPARE(txt->text, QString("image.png"));
}

void FilenameParserTest::testParseVariable()
{
	FilenameParser parser("%md5%");
	auto filename = parser.parseRoot();

	QCOMPARE(filename->exprs.count(), 1);

	auto var = dynamic_cast<FilenameNodeVariable*>(filename->exprs[0]);
	QVERIFY(var != nullptr);
	QCOMPARE(var->name, QString("md5"));
	QCOMPARE(var->opts.count(), 0);
}

void FilenameParserTest::testParseVariableWithOptions()
{
	FilenameParser parser("%md5:flag,opt=val%");
	auto filename = parser.parseRoot();

	QCOMPARE(filename->exprs.count(), 1);

	auto var = dynamic_cast<FilenameNodeVariable*>(filename->exprs[0]);
	QVERIFY(var != nullptr);
	QCOMPARE(var->name, QString("md5"));
	QCOMPARE(var->opts.count(), 2);
	QCOMPARE(var->opts.keys(), QStringList() << "flag" << "opt");
	QCOMPARE(var->opts["flag"], QString());
	QCOMPARE(var->opts["opt"], QString("val"));
}

void FilenameParserTest::testParseMixed()
{
	FilenameParser parser("out/%md5%.%ext%");
	auto filename = parser.parseRoot();

	QCOMPARE(filename->exprs.count(), 4);
}


void FilenameParserTest::testParseConditionTag()
{
	FilenameParser parser("\"my_tag\"");
	auto cond = parser.parseCondition();

	auto tagCond = dynamic_cast<FilenameNodeConditionTag*>(cond);
	QVERIFY(tagCond != nullptr);
	QCOMPARE(tagCond->tag.text(), QString("my_tag"));
}

void FilenameParserTest::testParseConditionToken()
{
	FilenameParser parser("%my_token%");
	auto cond = parser.parseCondition();

	auto tokenCond = dynamic_cast<FilenameNodeConditionToken*>(cond);
	QVERIFY(tokenCond != nullptr);
	QCOMPARE(tokenCond->token, QString("my_token"));
}

void FilenameParserTest::testParseConditionInvert()
{
	FilenameParser parser("!%my_token%");
	auto cond = parser.parseCondition();

	auto invertCond = dynamic_cast<FilenameNodeConditionInvert*>(cond);
	QVERIFY(invertCond != nullptr);

	auto tokenCond = dynamic_cast<FilenameNodeConditionToken*>(invertCond->node);
	QVERIFY(tokenCond != nullptr);
	QCOMPARE(tokenCond->token, QString("my_token"));
}

void FilenameParserTest::testParseConditionOperator()
{
	FilenameParser parser("\"my_tag\" & %my_token%");
	auto cond = parser.parseCondition();

	auto opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
	QVERIFY(opCond != nullptr);
	QCOMPARE(opCond->op, FilenameNodeConditionOp::Operator::And);

	auto left = dynamic_cast<FilenameNodeConditionTag*>(opCond->left);
	QVERIFY(left != nullptr);
	QCOMPARE(left->tag.text(), QString("my_tag"));

	auto right = dynamic_cast<FilenameNodeConditionToken*>(opCond->right);
	QVERIFY(right != nullptr);
	QCOMPARE(right->token, QString("my_token"));
}

void FilenameParserTest::testParseConditionMixedOperators()
{
	FilenameParser parser("\"my_tag\" | %some_token% & !%my_token%");
	auto cond = parser.parseCondition();

	auto opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
	QVERIFY(opCond != nullptr);
	QCOMPARE(opCond->op, FilenameNodeConditionOp::Operator::Or);

	auto right = dynamic_cast<FilenameNodeConditionOp*>(opCond->right);
	QVERIFY(right != nullptr);
	QCOMPARE(right->op, FilenameNodeConditionOp::Operator::And);

	auto invert = dynamic_cast<FilenameNodeConditionInvert*>(right->right);
	QVERIFY(invert != nullptr);
}

void FilenameParserTest::testParseConditionTagParenthesis()
{
	FilenameParser parser("(\"my_tag\")");
	auto cond = parser.parseCondition();

	auto tagCond = dynamic_cast<FilenameNodeConditionTag*>(cond);
	QVERIFY(tagCond != nullptr);
	QCOMPARE(tagCond->tag.text(), QString("my_tag"));
}

void FilenameParserTest::testParseConditionMixedParenthesis()
{
	FilenameParser parser("(\"my_tag\" | %some_token%) & %my_token%");
	auto cond = parser.parseCondition();

	auto opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
	QVERIFY(opCond != nullptr);
	QCOMPARE(opCond->op, FilenameNodeConditionOp::Operator::And);

	auto right = dynamic_cast<FilenameNodeConditionOp*>(opCond->left);
	QVERIFY(right != nullptr);
	QCOMPARE(right->op, FilenameNodeConditionOp::Operator::Or);
}


QTEST_MAIN(FilenameParserTest)
