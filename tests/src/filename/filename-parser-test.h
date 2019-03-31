#ifndef FILENAME_PARSER_TEST_H
#define FILENAME_PARSER_TEST_H

#include "test-suite.h"


class FilenameParserTest : public TestSuite
{
	Q_OBJECT

	private slots:
		// Basic
		void testParseEmpty();
		void testParseText();
		void testParseVariable();
		void testParseVariableWithOptions();
		void testParseMixed();

		// Conditionals
		void testParseConditional();
		void testParseConditionalLegacy();
		void testParseConditionalNoCondition();
		void testParseConditionalNoContent();
		void testParseConditionalUnterminated();

		// Condition
		void testParseConditionTag();
		void testParseConditionTagWithoutQuotes();
		void testParseConditionToken();
		void testParseConditionInvert();
		void testParseConditionOperator();
		void testParseConditionMixedOperators();
		void testParseConditionNoOperator();
		void testParseConditionTagParenthesis();
		void testParseConditionTagParenthesisUnclosed();
		void testParseConditionMixedParenthesis();
};

#endif // FILENAME_PARSER_TEST_H
