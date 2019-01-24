#ifndef FILENAME_PARSER_TEST_H
#define FILENAME_PARSER_TEST_H

#include "test-suite.h"


class FilenameParserTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testParseConditionTag();
		void testParseConditionToken();
		void testParseConditionOperator();
		void testParseConditionMixedOperators();
		void testParseConditionTagParenthesis();
		void testParseConditionMixedParenthesis();
};

#endif // FILENAME_PARSER_TEST_H
