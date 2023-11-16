#include <QMap>
#include <QString>
#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/ast/filename-node-conditional.h"
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-text.h"
#include "filename/ast/filename-node-variable.h"
#include "filename/filename-parser.h"
#include "catch.h"


TEST_CASE("FilenameParser")
{
	SECTION("ParseEmpty")
	{
		FilenameParser parser("");
		REQUIRE(parser.str() == QString());

		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.isEmpty());
	}

	SECTION("ParseText")
	{
		FilenameParser parser("image.png");
		REQUIRE(parser.str() == QString("image.png"));

		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 1);

		auto *txt = dynamic_cast<FilenameNodeText*>(filename->exprs[0]);
		REQUIRE(txt != nullptr);
		REQUIRE(txt->text == QString("image.png"));
	}

	SECTION("ParseVariable")
	{
		FilenameParser parser("%md5%");
		REQUIRE(parser.str() == QString("%md5%"));

		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 1);

		auto *var = dynamic_cast<FilenameNodeVariable*>(filename->exprs[0]);
		REQUIRE(var != nullptr);
		REQUIRE(var->name == QString("md5"));
		REQUIRE(var->opts.count() == 0);
	}

	SECTION("ParseVariableWithOptions")
	{
		FilenameParser parser("%md5:flag,opt=val%");
		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 1);

		auto *var = dynamic_cast<FilenameNodeVariable*>(filename->exprs[0]);
		REQUIRE(var != nullptr);
		REQUIRE(var->name == QString("md5"));
		REQUIRE(var->opts.count() == 2);
		REQUIRE(var->opts.keys() == QList<QString>() << "flag" << "opt");
		REQUIRE(var->opts["flag"] == QString());
		REQUIRE(var->opts["opt"] == QString("val"));
	}

	SECTION("ParseMixed")
	{
		FilenameParser parser("out/%md5%.%ext%");
		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 4);
	}

	SECTION("Escape")
	{
		SECTION("Using ^")
		{
			FilenameParser parser("^%md5^%");
			auto *filename = parser.parseRoot();
			REQUIRE(parser.error() == QString());

			REQUIRE(filename->exprs.count() == 1);

			auto *txt = dynamic_cast<FilenameNodeText *>(filename->exprs[0]);
			REQUIRE(txt != nullptr);
			REQUIRE(txt->text == QString("%md5%"));
		}

		SECTION("Escape ^ itself")
		{
			FilenameParser parser("hello ^^");
			auto *filename = parser.parseRoot();
			REQUIRE(parser.error() == QString());

			REQUIRE(filename->exprs.count() == 1);

			auto *txt = dynamic_cast<FilenameNodeText *>(filename->exprs[0]);
			REQUIRE(txt != nullptr);
			REQUIRE(txt->text == QString("hello ^"));
		}

		SECTION("XML characters")
		{
			FilenameParser parser("<<test>>");
			auto *filename = parser.parseRoot();
			REQUIRE(parser.error() == QString());

			REQUIRE(filename->exprs.count() == 1);

			auto *txt = dynamic_cast<FilenameNodeText *>(filename->exprs[0]);
			REQUIRE(txt != nullptr);
			REQUIRE(txt->text == QString("<test>"));
		}
	}

	SECTION("ParseConditional")
	{
		FilenameParser parser("out/<\"tag\"?some tag is present:%artist%>/image.png");
		auto filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 3);

		auto *txt1 = dynamic_cast<FilenameNodeText*>(filename->exprs[0]);
		REQUIRE(txt1 != nullptr);
		REQUIRE(txt1->text == QString("out/"));

		auto *conditional = dynamic_cast<FilenameNodeConditional*>(filename->exprs[1]);
		REQUIRE(conditional != nullptr);
		REQUIRE(conditional->ifTrue != nullptr);
		REQUIRE(conditional->ifFalse != nullptr);

		auto *cond = dynamic_cast<FilenameNodeConditionTag*>(conditional->condition);
		REQUIRE(cond != nullptr);
		REQUIRE(cond->tag.text() == QString("tag"));

		auto *ifTrue = dynamic_cast<FilenameNodeText*>(conditional->ifTrue);
		REQUIRE(ifTrue != nullptr);
		REQUIRE(ifTrue->text == QString("some tag is present"));

		auto *ifFalse = dynamic_cast<FilenameNodeVariable*>(conditional->ifFalse);
		REQUIRE(ifFalse != nullptr);
		REQUIRE(ifFalse->name == QString("artist"));

		auto *txt2 = dynamic_cast<FilenameNodeText*>(filename->exprs[2]);
		REQUIRE(txt2 != nullptr);
		REQUIRE(txt2->text == QString("/image.png"));
	}

	SECTION("Parse conditional with multiple expressions")
	{
		FilenameParser parser("out/<\"tag\"?%artist% and %copyright%:%artist% and %character%>/image.png");
		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 3);

		auto *txt1 = dynamic_cast<FilenameNodeText*>(filename->exprs[0]);
		REQUIRE(txt1 != nullptr);
		REQUIRE(txt1->text == QString("out/"));

		auto *conditional = dynamic_cast<FilenameNodeConditional*>(filename->exprs[1]);
		REQUIRE(conditional != nullptr);
		REQUIRE(conditional->ifTrue != nullptr);
		REQUIRE(conditional->ifFalse != nullptr);

		auto *cond = dynamic_cast<FilenameNodeConditionTag*>(conditional->condition);
		REQUIRE(cond != nullptr);
		REQUIRE(cond->tag.text() == QString("tag"));

		auto *ifTrue = dynamic_cast<FilenameNodeRoot*>(conditional->ifTrue);
		REQUIRE(ifTrue != nullptr);
		REQUIRE(ifTrue->exprs.count() == 3);

		auto *ifTrue1 = dynamic_cast<FilenameNodeVariable*>(ifTrue->exprs[0]);
		REQUIRE(ifTrue1 != nullptr);
		REQUIRE(ifTrue1->name == QString("artist"));

		auto *ifTrue2 = dynamic_cast<FilenameNodeText*>(ifTrue->exprs[1]);
		REQUIRE(ifTrue2 != nullptr);
		REQUIRE(ifTrue2->text == QString(" and "));

		auto *ifTrue3 = dynamic_cast<FilenameNodeVariable*>(ifTrue->exprs[2]);
		REQUIRE(ifTrue3 != nullptr);
		REQUIRE(ifTrue3->name == QString("copyright"));

		auto *ifFalse = dynamic_cast<FilenameNodeRoot*>(conditional->ifFalse);
		REQUIRE(ifFalse != nullptr);
		REQUIRE(ifFalse->exprs.count() == 3);

		auto *ifFalse1 = dynamic_cast<FilenameNodeVariable*>(ifFalse->exprs[0]);
		REQUIRE(ifFalse1 != nullptr);
		REQUIRE(ifFalse1->name == QString("artist"));

		auto *ifFalse2 = dynamic_cast<FilenameNodeText*>(ifFalse->exprs[1]);
		REQUIRE(ifFalse2 != nullptr);
		REQUIRE(ifFalse2->text == QString(" and "));

		auto *ifFalse3 = dynamic_cast<FilenameNodeVariable*>(ifFalse->exprs[2]);
		REQUIRE(ifFalse3 != nullptr);
		REQUIRE(ifFalse3->name == QString("character"));

		auto *txt2 = dynamic_cast<FilenameNodeText*>(filename->exprs[2]);
		REQUIRE(txt2 != nullptr);
		REQUIRE(txt2->text == QString("/image.png"));
	}

	SECTION("ParseConditionalLegacy")
	{
		FilenameParser parser("out/<some \"tag\" is present/>image.png");
		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 3);

		auto *txt1 = dynamic_cast<FilenameNodeText*>(filename->exprs[0]);
		REQUIRE(txt1 != nullptr);
		REQUIRE(txt1->text == QString("out/"));

		auto *conditional = dynamic_cast<FilenameNodeConditional*>(filename->exprs[1]);
		REQUIRE(conditional != nullptr);
		REQUIRE(conditional->ifTrue != nullptr);
		REQUIRE(conditional->ifFalse == nullptr);

		auto *cond = dynamic_cast<FilenameNodeConditionTag*>(conditional->condition);
		REQUIRE(cond != nullptr);
		REQUIRE(cond->tag.text() == QString("tag"));

		auto *ifTrue = dynamic_cast<FilenameNodeRoot*>(conditional->ifTrue);
		REQUIRE(ifTrue != nullptr);
		REQUIRE(ifTrue->exprs.count() == 3);

		auto *ifTrue1 = dynamic_cast<FilenameNodeText*>(ifTrue->exprs[0]);
		REQUIRE(ifTrue1 != nullptr);
		REQUIRE(ifTrue1->text == QString("some "));

		auto *ifTrue2 = dynamic_cast<FilenameNodeConditionTag*>(ifTrue->exprs[1]);
		REQUIRE(ifTrue2 != nullptr);
		REQUIRE(ifTrue2->tag.text() == QString("tag"));

		auto *ifTrue3 = dynamic_cast<FilenameNodeText*>(ifTrue->exprs[2]);
		REQUIRE(ifTrue3 != nullptr);
		REQUIRE(ifTrue3->text == QString(" is present/"));

		auto *txt2 = dynamic_cast<FilenameNodeText*>(filename->exprs[2]);
		REQUIRE(txt2 != nullptr);
		REQUIRE(txt2->text == QString("image.png"));
	}

	SECTION("Parse legacy conditional with trailing dash")
	{
		FilenameParser parser("<!%token% out->image.png");
		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 2);

		auto *conditional = dynamic_cast<FilenameNodeConditional*>(filename->exprs[0]);
		REQUIRE(conditional != nullptr);
		REQUIRE(conditional->ifTrue != nullptr);
		REQUIRE(conditional->ifFalse == nullptr);

		auto *invertCond = dynamic_cast<FilenameNodeConditionInvert*>(conditional->condition);
		REQUIRE(invertCond != nullptr);

		auto *cond = dynamic_cast<FilenameNodeConditionToken*>(invertCond->node);
		REQUIRE(cond != nullptr);
		REQUIRE(cond->name == QString("token"));

		auto *ifTrue = dynamic_cast<FilenameNodeRoot*>(conditional->ifTrue);
		REQUIRE(ifTrue != nullptr);
		REQUIRE(ifTrue->exprs.count() == 2);

		auto *ifTrue1Invert = dynamic_cast<FilenameNodeConditionInvert*>(ifTrue->exprs[0]);
		REQUIRE(ifTrue1Invert != nullptr);

		auto *ifTrue1 = dynamic_cast<FilenameNodeConditionToken*>(ifTrue1Invert->node);
		REQUIRE(ifTrue1 != nullptr);
		REQUIRE(ifTrue1->name == QString("token"));

		auto *ifTrue2 = dynamic_cast<FilenameNodeText*>(ifTrue->exprs[1]);
		REQUIRE(ifTrue2 != nullptr);
		REQUIRE(ifTrue2->text == QString(" out-"));

		auto *txt = dynamic_cast<FilenameNodeText*>(filename->exprs[1]);
		REQUIRE(txt != nullptr);
		REQUIRE(txt->text == QString("image.png"));
	}

	SECTION("Parse legacy conditional with options")
	{
		FilenameParser parser("<%md5:flag,opt=val%>image.png");
		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 2);

		auto *conditional = dynamic_cast<FilenameNodeConditional*>(filename->exprs[0]);
		REQUIRE(conditional != nullptr);
		REQUIRE(conditional->ifTrue != nullptr);
		REQUIRE(conditional->ifFalse == nullptr);

		auto *cond = dynamic_cast<FilenameNodeConditionToken*>(conditional->condition);
		REQUIRE(cond != nullptr);
		REQUIRE(cond->name == QString("md5"));
		REQUIRE(cond->opts.count() == 2);
		REQUIRE(cond->opts.keys() == QList<QString>() << "flag" << "opt");
		REQUIRE(cond->opts["flag"] == QString());
		REQUIRE(cond->opts["opt"] == QString("val"));

		auto *ifTrue = dynamic_cast<FilenameNodeConditionToken*>(conditional->ifTrue);
		REQUIRE(ifTrue != nullptr);
		REQUIRE(ifTrue->name == QString("md5"));
		REQUIRE(ifTrue->opts.count() == 2);
		REQUIRE(ifTrue->opts.keys() == QList<QString>() << "flag" << "opt");
		REQUIRE(ifTrue->opts["flag"] == QString());
		REQUIRE(ifTrue->opts["opt"] == QString("val"));

		auto *txt = dynamic_cast<FilenameNodeText*>(filename->exprs[1]);
		REQUIRE(txt != nullptr);
		REQUIRE(txt->text == QString("image.png"));
	}

	SECTION("ParseConditionalLegacyDash")
	{
		FilenameParser parser("<\"tag\"-out/>image.png");
		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 2);

		auto *conditional = dynamic_cast<FilenameNodeConditional*>(filename->exprs[0]);
		REQUIRE(conditional != nullptr);
		REQUIRE(conditional->ifTrue != nullptr);
		REQUIRE(conditional->ifFalse == nullptr);

		auto *cond = dynamic_cast<FilenameNodeConditionTag*>(conditional->condition);
		REQUIRE(cond != nullptr);
		REQUIRE(cond->tag.text() == QString("tag"));

		auto *ifTrue = dynamic_cast<FilenameNodeRoot*>(conditional->ifTrue);
		REQUIRE(ifTrue != nullptr);
		REQUIRE(ifTrue->exprs.count() == 2);

		auto *ifTrue1 = dynamic_cast<FilenameNodeConditionTag*>(ifTrue->exprs[0]);
		REQUIRE(ifTrue1 != nullptr);
		REQUIRE(ifTrue1->tag.text() == QString("tag"));

		auto *ifTrue2 = dynamic_cast<FilenameNodeText*>(ifTrue->exprs[1]);
		REQUIRE(ifTrue2 != nullptr);
		REQUIRE(ifTrue2->text == QString("-out/"));

		auto *txt = dynamic_cast<FilenameNodeText*>(filename->exprs[1]);
		REQUIRE(txt != nullptr);
		REQUIRE(txt->text == QString("image.png"));
	}

	SECTION("ParseConditionalNoCondition")
	{
		FilenameParser parser("<no condition here>");
		parser.parseRoot();

		REQUIRE(parser.error() == QString("No condition found in conditional"));
	}

	SECTION("ParseConditionalNoContent")
	{
		FilenameParser parser("<%condition%:?>");
		parser.parseRoot();

		REQUIRE(parser.error() == QString("Expected '?' after condition"));
	}

	SECTION("ParseConditionalUnterminated")
	{
		FilenameParser parser("out/<\"tag\"?some tag is present:%artist%");
		parser.parseRoot();

		REQUIRE(parser.error() == QString("Expected '>' at the end of conditional"));
	}

	SECTION("ParseConditionalWithInvalidCharacters")
	{
		FilenameParser parser("<\"a?b:c\"?yes>");
		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 1);

		auto *conditional = dynamic_cast<FilenameNodeConditional*>(filename->exprs[0]);
		REQUIRE(conditional != nullptr);
		REQUIRE(conditional->ifTrue != nullptr);
		REQUIRE(conditional->ifFalse == nullptr);

		auto *cond = dynamic_cast<FilenameNodeConditionTag*>(conditional->condition);
		REQUIRE(cond != nullptr);
		REQUIRE(cond->tag.text() == QString("a?b:c"));

		auto *ifTrue = dynamic_cast<FilenameNodeText*>(conditional->ifTrue);
		REQUIRE(ifTrue != nullptr);
		REQUIRE(ifTrue->text == QString("yes"));
	}

	SECTION("Parse conditional with parenthesis")
	{
		FilenameParser parser(R"(<"a" & ("b" | "c") ? yes>)");
		auto *filename = parser.parseRoot();
		REQUIRE(parser.error() == QString());

		REQUIRE(filename->exprs.count() == 1);

		auto *conditional = dynamic_cast<FilenameNodeConditional*>(filename->exprs[0]);
		REQUIRE(conditional != nullptr);
		REQUIRE(conditional->ifTrue != nullptr);
		REQUIRE(conditional->ifFalse == nullptr);

		auto *cond = dynamic_cast<FilenameNodeConditionOp*>(conditional->condition);
		REQUIRE(cond != nullptr);
		REQUIRE(cond->op == FilenameNodeConditionOp::Operator::And);

		auto *right = dynamic_cast<FilenameNodeConditionOp*>(cond->right);
		REQUIRE(right != nullptr);
		REQUIRE(right->op == FilenameNodeConditionOp::Operator::Or);
	}


	SECTION("ParseConditionTag")
	{
		FilenameParser parser("\"my_tag\"");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *tagCond = dynamic_cast<FilenameNodeConditionTag*>(cond);
		REQUIRE(tagCond != nullptr);
		REQUIRE(tagCond->tag.text() == QString("my_tag"));
	}

	SECTION("ParseConditionTagWithoutQuotes")
	{
		FilenameParser parser("my_tag");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *tagCond = dynamic_cast<FilenameNodeConditionTag*>(cond);
		REQUIRE(tagCond != nullptr);
		REQUIRE(tagCond->tag.text() == QString("my_tag"));
	}

	SECTION("Parse condition tag without quotes and with parenthesis")
	{
		FilenameParser parser("my_tag_(test)");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *tagCond = dynamic_cast<FilenameNodeConditionTag*>(cond);
		REQUIRE(tagCond != nullptr);
		REQUIRE(tagCond->tag.text() == QString("my_tag_(test)"));
	}

	SECTION("ParseConditionToken")
	{
		FilenameParser parser("%my_token%");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *tokenCond = dynamic_cast<FilenameNodeConditionToken*>(cond);
		REQUIRE(tokenCond != nullptr);
		REQUIRE(tokenCond->name == QString("my_token"));
	}

	SECTION("ParseConditionInvert")
	{
		FilenameParser parser("!%my_token%");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *invertCond = dynamic_cast<FilenameNodeConditionInvert*>(cond);
		REQUIRE(invertCond != nullptr);

		auto *tokenCond = dynamic_cast<FilenameNodeConditionToken*>(invertCond->node);
		REQUIRE(tokenCond != nullptr);
		REQUIRE(tokenCond->name == QString("my_token"));
	}

	SECTION("ParseConditionOperator")
	{
		FilenameParser parser("\"my_tag\" & %my_token%");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
		REQUIRE(opCond != nullptr);
		REQUIRE(opCond->op == FilenameNodeConditionOp::Operator::And);

		auto *left = dynamic_cast<FilenameNodeConditionTag*>(opCond->left);
		REQUIRE(left != nullptr);
		REQUIRE(left->tag.text() == QString("my_tag"));

		auto *right = dynamic_cast<FilenameNodeConditionToken*>(opCond->right);
		REQUIRE(right != nullptr);
		REQUIRE(right->name == QString("my_token"));
	}

	SECTION("ParseConditionMixedOperators")
	{
		FilenameParser parser("\"my_tag\" | %some_token% & !%my_token%");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
		REQUIRE(opCond != nullptr);
		REQUIRE(opCond->op == FilenameNodeConditionOp::Operator::Or);

		auto *right = dynamic_cast<FilenameNodeConditionOp*>(opCond->right);
		REQUIRE(right != nullptr);
		REQUIRE(right->op == FilenameNodeConditionOp::Operator::And);

		auto *invert = dynamic_cast<FilenameNodeConditionInvert*>(right->right);
		REQUIRE(invert != nullptr);
	}

	SECTION("ParseConditionMixedOperatorsParenthesis")
	{
		FilenameParser parser("(\"my_tag\" | %some_token%) & %my_token%");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
		REQUIRE(opCond != nullptr);
		REQUIRE(opCond->op == FilenameNodeConditionOp::Operator::And);

		auto *left = dynamic_cast<FilenameNodeConditionOp*>(opCond->left);
		REQUIRE(left != nullptr);
		REQUIRE(left->op == FilenameNodeConditionOp::Operator::Or);
	}

	SECTION("ParseConditionNoOperator")
	{
		FilenameParser parser("\"my_tag\" %my_token%");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
		REQUIRE(opCond != nullptr);
		REQUIRE(opCond->op == FilenameNodeConditionOp::Operator::And);

		auto *left = dynamic_cast<FilenameNodeConditionTag*>(opCond->left);
		REQUIRE(left != nullptr);
		REQUIRE(left->tag.text() == QString("my_tag"));

		auto *right = dynamic_cast<FilenameNodeConditionToken*>(opCond->right);
		REQUIRE(right != nullptr);
		REQUIRE(right->name == QString("my_token"));
	}

	SECTION("ParseConditionTagParenthesis")
	{
		FilenameParser parser("(\"my_tag\")");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *tagCond = dynamic_cast<FilenameNodeConditionTag*>(cond);
		REQUIRE(tagCond != nullptr);
		REQUIRE(tagCond->tag.text() == QString("my_tag"));
	}

	SECTION("ParseConditionTagParenthesisUnclosed")
	{
		FilenameParser parser("(\"my_tag\"");
		parser.parseCondition();

		REQUIRE(parser.error() == QString("Expected ')' after condition in parenthesis"));
	}

	SECTION("ParseConditionMixedParenthesis")
	{
		FilenameParser parser("(\"my_tag\" | %some_token%) & %my_token%");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
		REQUIRE(opCond != nullptr);
		REQUIRE(opCond->op == FilenameNodeConditionOp::Operator::And);

		auto *right = dynamic_cast<FilenameNodeConditionOp*>(opCond->left);
		REQUIRE(right != nullptr);
		REQUIRE(right->op == FilenameNodeConditionOp::Operator::Or);
	}

	SECTION("ParseConditionInvalidCharacters")
	{
		FilenameParser parser("\"a|b&c\" & %my_token%");
		auto *cond = parser.parseCondition();
		REQUIRE(parser.error() == QString());

		auto *opCond = dynamic_cast<FilenameNodeConditionOp*>(cond);
		REQUIRE(opCond != nullptr);
		REQUIRE(opCond->op == FilenameNodeConditionOp::Operator::And);

		auto *left = dynamic_cast<FilenameNodeConditionTag*>(opCond->left);
		REQUIRE(left != nullptr);
		REQUIRE(left->tag.text() == QString("a|b&c"));

		auto *right = dynamic_cast<FilenameNodeConditionToken*>(opCond->right);
		REQUIRE(right != nullptr);
		REQUIRE(right->name == QString("my_token"));
	}
}
