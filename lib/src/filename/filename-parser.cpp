#include "filename/filename-parser.h"
#include <QStack>
#include "filename/ast/filename-node-condition.h"
#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"


FilenameParser::FilenameParser(QString str)
	: m_str(std::move(str)), m_index(0)
{}


QChar FilenameParser::peek()
{
	if (finished()) {
		return {0};
	}

	QChar c = m_str[m_index];
	if (c.isSpace()) {
		m_index++;
		return peek();
	}

	return c;
}

bool FilenameParser::finished()
{
	return m_index >= m_str.count();
}


FilenameNodeCondition *FilenameParser::parseCondition()
{
	FilenameNodeCondition *lhs;

	// Parenthesis
	QChar p = peek();
	if (p == '(') {
		m_index++; // (

		lhs = parseCondition();
		if (peek() != ')') {
			return nullptr;
		}

		m_index++; // )
	} else {
		lhs = parseSingleCondition();
	}

	QStack<QChar> opsStack;
	QStack<FilenameNodeCondition*> termStack;

	while (!finished()) {
		QChar c = peek();
		if (c != '&' && c != '|') {
			break;
		}

		termStack.push(lhs);

		int prec = (c == '&' ? 2 : 1);

		while (!opsStack.isEmpty()) {
			QChar stackC = opsStack.top();
			int stackPrec = (stackC == '&' ? 2 : 1);

			if (prec > stackPrec) {
				break;
			}

			FilenameNodeCondition *operand2 = termStack.pop();
			FilenameNodeCondition *operand1 = termStack.pop();

			QChar opC = opsStack.pop();
			auto op = opC == '&' ? FilenameNodeConditionOp::And : FilenameNodeConditionOp::Or;
			auto expr = new FilenameNodeConditionOp(op, operand1, operand2);
			termStack.push(expr);
		}

		opsStack.push(c);
		m_index++;

		termStack.push(parseSingleCondition());
	}

	while (!opsStack.isEmpty()) {
		FilenameNodeCondition *operand2 = termStack.pop();
		FilenameNodeCondition *operand1 = termStack.pop();

		QChar opC = opsStack.pop();
		auto op = opC == '&' ? FilenameNodeConditionOp::And : FilenameNodeConditionOp::Or;
		auto expr = new FilenameNodeConditionOp(op, operand1, operand2);
		termStack.push(expr);
	}

	auto term = lhs;

	if (!termStack.isEmpty()) {
		term = termStack.pop();
	}

	return term;
}

FilenameNodeCondition *FilenameParser::parseSingleCondition()
{
	QChar c = peek();

	if (c == '%') {
		return parseConditionToken();
	}
	if (c == '"') {
		return parseConditionTag();
	}

	return nullptr;
}

FilenameNodeConditionTag *FilenameParser::parseConditionTag()
{
	m_index++; // "

	int end = m_str.indexOf('"', m_index);
	if (end == -1) {
		return nullptr;
	}

	QString tag = m_str.mid(m_index, end - m_index);
	m_index = end;

	m_index++; // "

	return new FilenameNodeConditionTag(Tag(tag));
}

FilenameNodeConditionToken *FilenameParser::parseConditionToken()
{
	m_index++; // %

	int end = m_str.indexOf('%', m_index);
	if (end == -1) {
		return nullptr;
	}

	QString token = m_str.mid(m_index, end - m_index);
	m_index = end;

	m_index++; // %

	return new FilenameNodeConditionToken(token);
}
