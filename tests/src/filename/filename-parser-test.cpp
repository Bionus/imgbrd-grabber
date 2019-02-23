#include "filename-parser-test.h"
#include <QMap>
#include <QString>
#include <QtTest>
#include "filename/filename-parser.h"
#include "filename/ast/filename-node-conditional.h"
#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-text.h"
#include "filename/ast/filename-node-variable.h"


void FilenameParserTest::testParseEmpty()
{
	FilenameParser parser("");
	auto filename = parser.parseRoot();
	QVERIFY(parser.error().isEmpty());

	QVERIFY(filename->exprs.isEmpty());
}

void FilenameParserTest::testParseText()
{
	FilenameParser parser("image.png");
	auto filename = parser.parseRoot();
	QVERIFY(parser.error().isEmpty());

	QCOMPARE(filename->exprs.count(), 1);

	auto txt = dynamic_cast<FilenameNodeText*>(filename->exprs[0]);
	QVERIFY(txt != nullptr);
	QCOMPARE(txt->text, QString("image.png"));
}

void FilenameParserTest::testParseVariable()
{
	FilenameParser parser("%md5%");
	auto filename = parser.parseRoot();
	QVERIFY(parser.error().isEmpty());

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
	QVERIFY(parser.error().isEmpty());

	QCOMPARE(filename->exprs.count(), 1);

	auto var = dynamic_cast<FilenameNodeVariable*>(filename->exprs[0]);
	QVERIFY(var != nullptr);
	QCOMPARE(var->name, QString("md5"));
	QCOMPARE(var->opts.count(), 2);
	QCOMPARE(var->opts.keys(), QList<QString>() << "flag" << "opt");
	QCOMPARE(var->opts["flag"], QString());
	QCOMPARE(var->opts["opt"], QString("val"));
}

void FilenameParserTest::testParseMixed()
{
	FilenameParser parser("out/%md5%.%ext%");
	auto filename = parser.parseRoot();
	QVERIFY(parser.error().isEmpty());

	QCOMPARE(filename->exprs.count(), 4);
}


void FilenameParserTest::testParseConditional()
{
	FilenameParser parser("out/<\"tag\"?some tag is present:%artist%>/image.png");
	auto filename = parser.parseRoot();
	QVERIFY(parser.error().isEmpty());

	QCOMPARE(filename->exprs.count(), 3);

	auto txt1 = dynamic_cast<FilenameNodeText*>(filename->exprs[0]);
	QVERIFY(txt1 != nullptr);
	QCOMPARE(txt1->text, QString("out/"));

	auto conditional = dynamic_cast<FilenameNodeConditional*>(filename->exprs[1]);
	QVERIFY(conditional != nullptr);
	QVERIFY(conditional->ifTrue != nullptr);
	QVERIFY(conditional->ifFalse != nullptr);

	auto cond = dynamic_cast<FilenameNodeConditionTag*>(conditional->condition);
	QVERIFY(cond != nullptr);
	QCOMPARE(cond->tag.text(), QString("tag"));

	auto ifTrue = dynamic_cast<FilenameNodeText*>(conditional->ifTrue);
	QVERIFY(ifTrue != nullptr);
	QCOMPARE(ifTrue->text, QString("some tag is present"));

	auto ifFalse = dynamic_cast<FilenameNodeVariable*>(conditional->ifFalse);
	QVERIFY(ifFalse != nullptr);
	QCOMPARE(ifFalse->name, QString("artist"));

	auto txt2 = dynamic_cast<FilenameNodeText*>(filename->exprs[2]);
	QVERIFY(txt2 != nullptr);
	QCOMPARE(txt2->text, QString("/image.png"));
}

void FilenameParserTest::testParseConditionalLegacy()
{
	FilenameParser parser("out/<some \"tag\" is present/>image.png");
	auto filename = parser.parseRoot();
	QVERIFY(parser.error().isEmpty());

	QCOMPARE(filename->exprs.count(), 3);

	auto txt1 = dynamic_cast<FilenameNodeText*>(filename->exprs[0]);
	QVERIFY(txt1 != nullptr);
	QCOMPARE(txt1->text, QString("out/"));

	auto conditional = dynamic_cast<FilenameNodeConditional*>(filename->exprs[1]);
	QVERIFY(conditional != nullptr);
	QVERIFY(conditional->ifTrue != nullptr);
	QVERIFY(conditional->ifFalse == nullptr);

	auto cond = dynamic_cast<FilenameNodeConditionTag*>(conditional->condition);
	QVERIFY(cond != nullptr);
	QCOMPARE(cond->tag.text(), QString("tag"));

	auto ifTrue = dynamic_cast<FilenameNodeRoot*>(conditional->ifTrue);
	QVERIFY(ifTrue != nullptr);
	QCOMPARE(ifTrue->exprs.count(), 3);

	auto ifTrue1 = dynamic_cast<FilenameNodeText*>(ifTrue->exprs[0]);
	QVERIFY(ifTrue1 != nullptr);
	QCOMPARE(ifTrue1->text, QString("some "));

	auto ifTrue2 = dynamic_cast<FilenameNodeConditionTag*>(ifTrue->exprs[1]);
	QVERIFY(ifTrue2 != nullptr);
	QCOMPARE(ifTrue2->tag.text(), QString("tag"));

	auto ifTrue3 = dynamic_cast<FilenameNodeText*>(ifTrue->exprs[2]);
	QVERIFY(ifTrue3 != nullptr);
	QCOMPARE(ifTrue3->text, QString(" is present/"));

	auto txt2 = dynamic_cast<FilenameNodeText*>(filename->exprs[2]);
	QVERIFY(txt2 != nullptr);
	QCOMPARE(txt2->text, QString("image.png"));
}

void FilenameParserTest::testParseConditionalNoCondition()
{
	FilenameParser parser("<no condition here>");
	parser.parseRoot();

	QCOMPARE(parser.error(), QString("No condition found in conditional"));
}

void FilenameParserTest::testParseConditionalNoContent()
{
	FilenameParser parser("<%condition%:?>");
	parser.parseRoot();

	QCOMPARE(parser.error(), QString("Expected '?' after condition"));
}

void FilenameParserTest::testParseConditionalUnterminated()
{
	FilenameParser parser("out/<\"tag\"?some tag is present:%artist%");
	parser.parseRoot();

	QCOMPARE(parser.error(), QString("Expected '>' at the end of contional"));
}


void FilenameParserTest::testParseConditionTag()
{
	FilenameParser parser("\"my_tag\"");
	auto cond = parser.parseCondition();
	QVERIFY(parser.error().isEmpty());

	auto tagCond = dynamic_cast<FilenameNodeConditionTag*>(cond);
	QVERIFY(tagCond != nullptr);
	QCOMPARE(tagCond->tag.text(), QString("my_tag"));
}

void FilenameParserTest::testParseConditionTagWithoutQuotes()
{
	FilenameParser parser("my_tag");
	auto cond = parser.parseCondition();
	QVERIFY(parser.error().isEmpty());

	auto tagCond = dynamic_cast<FilenameNodeConditionTag*>(cond);
	QVERIFY(tagCond != nullptr);
	QCOMPARE(tagCond->tag.text(), QString("my_tag"));
}

void FilenameParserTest::testParseConditionToken()
{
	FilenameParser parser("%my_token%");
	auto cond = parser.parseCondition();
	QVERIFY(parser.error().isEmpty());

	auto tokenCond = dynamic_cast<FilenameNodeConditionToken*>(cond);
	QVERIFY(tokenCond != nullptr);
	QCOMPARE(tokenCond->token, QString("my_token"));
}

void FilenameParserTest::testParseConditionInvert()
{
	FilenameParser parser("!%my_token%");
	auto cond = parser.parseCondition();
	QVERIFY(parser.error().isEmpty());

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
	QVERIFY(parser.error().isEmpty());

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
	QVERIFY(parser.error().isEmpty());

	auto opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
	QVERIFY(opCond != nullptr);
	QCOMPARE(opCond->op, FilenameNodeConditionOp::Operator::Or);

	auto right = dynamic_cast<FilenameNodeConditionOp*>(opCond->right);
	QVERIFY(right != nullptr);
	QCOMPARE(right->op, FilenameNodeConditionOp::Operator::And);

	auto invert = dynamic_cast<FilenameNodeConditionInvert*>(right->right);
	QVERIFY(invert != nullptr);
}

void FilenameParserTest::testParseConditionNoOperator()
{
	FilenameParser parser("\"my_tag\" %my_token%");
	auto cond = parser.parseCondition();
	QVERIFY(parser.error().isEmpty());

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

void FilenameParserTest::testParseConditionTagParenthesis()
{
	FilenameParser parser("(\"my_tag\")");
	auto cond = parser.parseCondition();
	QVERIFY(parser.error().isEmpty());

	auto tagCond = dynamic_cast<FilenameNodeConditionTag*>(cond);
	QVERIFY(tagCond != nullptr);
	QCOMPARE(tagCond->tag.text(), QString("my_tag"));
}

void FilenameParserTest::testParseConditionTagParenthesisUnclosed()
{
	FilenameParser parser("(\"my_tag\"");
	parser.parseCondition();

	QCOMPARE(parser.error(), QString("Expected ')' after condition in parenthesis"));
}

void FilenameParserTest::testParseConditionMixedParenthesis()
{
	FilenameParser parser("(\"my_tag\" | %some_token%) & %my_token%");
	auto cond = parser.parseCondition();
	QVERIFY(parser.error().isEmpty());

	auto opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
	QVERIFY(opCond != nullptr);
	QCOMPARE(opCond->op, FilenameNodeConditionOp::Operator::And);

	auto right = dynamic_cast<FilenameNodeConditionOp*>(opCond->left);
	QVERIFY(right != nullptr);
	QCOMPARE(right->op, FilenameNodeConditionOp::Operator::Or);
}


QTEST_MAIN(FilenameParserTest)
