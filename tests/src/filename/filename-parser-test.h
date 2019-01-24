#ifndef FILENAME_PARSER_TEST_H
#define FILENAME_PARSER_TEST_H

#include "test-suite.h"


class FilenameParserTest : public TestSuite
{
	Q_OBJECT

	private slots:
		// Basic
		void testParseText();
		void testParseVariable();
		void testParseVariableWithOptions();
		void testParseMixed();

		// Conditionals
		void testParseConditional();
		void testParseConditionalLegacy();

		// Condition
		void testParseConditionTag();
		void testParseConditionToken();
		void testParseConditionInvert();
		void testParseConditionOperator();
		void testParseConditionMixedOperators();
		void testParseConditionTagParenthesis();
		void testParseConditionMixedParenthesis();
};

#endif // FILENAME_PARSER_TEST_H
