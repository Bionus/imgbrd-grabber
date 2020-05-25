#include <QMap>
#include <QSettings>
#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-node-condition-javascript.h"
#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/filename-condition-visitor.h"
#include "loader/token.h"
#include "catch.h"


using NInvert = FilenameNodeConditionInvert;
using NOp = FilenameNodeConditionOp;
using NTag = FilenameNodeConditionTag;
using NToken = FilenameNodeConditionToken;

TEST_CASE("FilenameConditionVisitor")
{
	SECTION("Tag")
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
		REQUIRE(FilenameConditionVisitor(tokensWithTag, &settings).run(condition));
		REQUIRE(!FilenameConditionVisitor(tokensWithoutTag, &settings).run(condition));
		REQUIRE(!FilenameConditionVisitor(tokensWithoutAnyTag, &settings).run(condition));
	}

	SECTION("Token")
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
		REQUIRE(FilenameConditionVisitor(tokensWithToken, &settings).run(condition));
		REQUIRE(!FilenameConditionVisitor(tokensWithEmptyToken, &settings).run(condition));
		REQUIRE(!FilenameConditionVisitor(tokensWithoutToken, &settings).run(condition));
	}

	SECTION("OperatorOr")
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
		REQUIRE(FilenameConditionVisitor(tokens, &settings).run(trueTrue));
		REQUIRE(FilenameConditionVisitor(tokens, &settings).run(trueFalse));
		REQUIRE(FilenameConditionVisitor(tokens, &settings).run(falseTrue));
		REQUIRE(!FilenameConditionVisitor(tokens, &settings).run(falseFalse));
	}

	SECTION("OperatorAnd")
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
		REQUIRE(FilenameConditionVisitor(tokens, &settings).run(trueTrue));
		REQUIRE(!FilenameConditionVisitor(tokens, &settings).run(trueFalse));
		REQUIRE(!FilenameConditionVisitor(tokens, &settings).run(falseTrue));
		REQUIRE(!FilenameConditionVisitor(tokens, &settings).run(falseFalse));
	}

	SECTION("MixedOperators")
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
		REQUIRE(FilenameConditionVisitor(tokens, &settings).run(left));

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
		REQUIRE(!FilenameConditionVisitor(tokens, &settings).run(right));
	}

	SECTION("Invert")
	{
		QMap<QString, Token> tokens = {
			{ "allos", Token(QStringList() << "tag1" << "tag2" << "tag3") },
		};

		auto validTag = new NTag(Tag("tag1"));
		auto invalidToken = new NToken("not_found");

		auto validOp = new NOp(NOp::Operator::Or, new NTag(Tag("tag1")), new NToken("not_found"));
		auto invalidOp = new NOp(NOp::Operator::And, new NTag(Tag("tag1")), new NToken("not_found"));

		QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
		REQUIRE(!FilenameConditionVisitor(tokens, &settings).run(NInvert(validTag)));
		REQUIRE(FilenameConditionVisitor(tokens, &settings).run(NInvert(invalidToken)));
		REQUIRE(!FilenameConditionVisitor(tokens, &settings).run(NInvert(validOp)));
		REQUIRE(FilenameConditionVisitor(tokens, &settings).run(NInvert(invalidOp)));
	}

	SECTION("JavaScript")
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
		REQUIRE(FilenameConditionVisitor(tokensWithToken, &settings).run(condition));
		REQUIRE(!FilenameConditionVisitor(tokensWithEmptyToken, &settings).run(condition));
		REQUIRE(!FilenameConditionVisitor(tokensWithoutToken, &settings).run(condition));
	}
}
