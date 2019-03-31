#include "filename-condition-visitor-test.h"
#include <QMap>
#include <QtTest>
#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-node-condition-javascript.h"
#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/filename-condition-visitor.h"
#include "loader/token.h"


using NInvert = FilenameNodeConditionInvert;
using NOp = FilenameNodeConditionOp;
using NTag = FilenameNodeConditionTag;
using NToken = FilenameNodeConditionToken;

void FilenameConditionVisitorTest::testTag()
{
	NTag condition(Tag("my_tag"));

	QMap<QString, Token> tokensWithTag = {
		{ "allos", Token(QStringList() << "tag1" << "tag2" << "my_tag" << "tag3") },
	};
	QMap<QString, Token> tokensWithoutTag = {
		{ "allos", Token(QStringList() << "tag1" << "tag2" << "tag3") },
	};
	QMap<QString, Token> tokensWithoutAnyTag;

	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
	QCOMPARE(FilenameConditionVisitor(tokensWithTag, &settings).run(condition), true);
	QCOMPARE(FilenameConditionVisitor(tokensWithoutTag, &settings).run(condition), false);
	QCOMPARE(FilenameConditionVisitor(tokensWithoutAnyTag, &settings).run(condition), false);
}

void FilenameConditionVisitorTest::testToken()
{
	NToken condition("my_token");

	QMap<QString, Token> tokensWithToken = {
		{ "my_token", Token("not_empty") },
	};
	QMap<QString, Token> tokensWithEmptyToken = {
		{ "my_token", Token("") },
	};
	QMap<QString, Token> tokensWithoutToken;

	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
	QCOMPARE(FilenameConditionVisitor(tokensWithToken, &settings).run(condition), true);
	QCOMPARE(FilenameConditionVisitor(tokensWithEmptyToken, &settings).run(condition), false);
	QCOMPARE(FilenameConditionVisitor(tokensWithoutToken, &settings).run(condition), false);
}

void FilenameConditionVisitorTest::testOperatorOr()
{
	QMap<QString, Token> tokens = {
		{ "allos", Token(QStringList() << "tag1" << "tag2" << "tag3") },
	};

	auto op = NOp::Operator::Or;
	NOp trueTrue(op, new NTag(Tag("tag1")), new NTag(Tag("tag1")));
	NOp trueFalse(op, new NTag(Tag("tag1")), new NTag(Tag("not_found")));
	NOp falseTrue(op, new NTag(Tag("not_found")), new NTag(Tag("tag1")));
	NOp falseFalse(op, new NTag(Tag("not_found")), new NTag(Tag("not_found")));

	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(trueTrue), true);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(trueFalse), true);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(falseTrue), true);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(falseFalse), false);
}

void FilenameConditionVisitorTest::testOperatorAnd()
{
	QMap<QString, Token> tokens = {
		{ "allos", Token(QStringList() << "tag1" << "tag2" << "tag3") },
	};

	auto op = NOp::Operator::And;
	NOp trueTrue(op, new NTag(Tag("tag1")), new NTag(Tag("tag1")));
	NOp trueFalse(op, new NTag(Tag("tag1")), new NTag(Tag("not_found")));
	NOp falseTrue(op, new NTag(Tag("not_found")), new NTag(Tag("tag1")));
	NOp falseFalse(op, new NTag(Tag("not_found")), new NTag(Tag("not_found")));

	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(trueTrue), true);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(trueFalse), false);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(falseTrue), false);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(falseFalse), false);
}

void FilenameConditionVisitorTest::testMixedOperators()
{
	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);

	QMap<QString, Token> tokens = {
		{ "allos", Token(QStringList() << "tag1" << "tag2" << "tag3") },
	};

	// A || (!B && C), A = true, B = false, C = false => true
	NOp left(
		NOp::Operator::Or,
		new NTag(Tag("tag1")),
		new NOp(
			NOp::Operator::And,
			new NInvert(new NTag(Tag("not_found"))),
			new NTag(Tag("not_found"))
		)
	);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(left), true);

	// (A || !B) && C, A = true, B = false, C = false => false
	NOp right(
		NOp::Operator::And,
		new NOp(
			NOp::Operator::Or,
			new NTag(Tag("tag1")),
			new NInvert(new NTag(Tag("not_found")))
		),
		new NTag(Tag("not_found"))
	);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(right), false);
}

void FilenameConditionVisitorTest::testInvert()
{
	QMap<QString, Token> tokens = {
		{ "allos", Token(QStringList() << "tag1" << "tag2" << "tag3") },
	};

	auto validTag = new NTag(Tag("tag1"));
	auto invalidToken = new NToken("not_found");

	auto validOp = new NOp(NOp::Operator::Or, new NTag(Tag("tag1")), new NToken("not_found"));
	auto invalidOp = new NOp(NOp::Operator::And, new NTag(Tag("tag1")), new NToken("not_found"));

	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(NInvert(validTag)), false);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(NInvert(invalidToken)), true);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(NInvert(validOp)), false);
	QCOMPARE(FilenameConditionVisitor(tokens, &settings).run(NInvert(invalidOp)), true);
}

void FilenameConditionVisitorTest::testJavaScript()
{
	FilenameNodeConditionJavaScript condition("typeof my_token !== 'undefined' && my_token.length > 0");

	QMap<QString, Token> tokensWithToken = {
		{ "my_token", Token("not_empty") },
	};
	QMap<QString, Token> tokensWithEmptyToken = {
		{ "my_token", Token("") },
	};
	QMap<QString, Token> tokensWithoutToken;

	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
	QCOMPARE(FilenameConditionVisitor(tokensWithToken, &settings).run(condition), true);
	QCOMPARE(FilenameConditionVisitor(tokensWithEmptyToken, &settings).run(condition), false);
	QCOMPARE(FilenameConditionVisitor(tokensWithoutToken, &settings).run(condition), false);
}


QTEST_MAIN(FilenameConditionVisitorTest)
