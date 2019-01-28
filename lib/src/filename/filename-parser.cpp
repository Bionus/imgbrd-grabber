#include "filename/filename-parser.h"
#include <QChar>
#include <QList>
#include <QStack>
#include "filename/ast/filename-node-condition.h"
#include "filename/ast/filename-node-conditional.h"
#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-text.h"
#include "filename/ast/filename-node-variable.h"


FilenameParser::FilenameParser(QString str)
	: m_str(std::move(str)), m_index(0)
{}

QString FilenameParser::error() const
{
	return m_error;
}


FilenameNodeRoot *FilenameParser::parseRoot()
{
	try {
		return parseRootNode();
	} catch (const std::runtime_error &e) {
		m_error = e.what();
		return nullptr;
	}
}

FilenameNodeCondition *FilenameParser::parseCondition()
{
	try {
		return parseConditionNode();
	} catch (const std::runtime_error &e) {
		m_error = e.what();
		return nullptr;
	}
}


QChar FilenameParser::peek()
{
	return m_str[m_index];
}

bool FilenameParser::finished()
{
	return m_index >= m_str.count();
}

void FilenameParser::skipSpaces()
{
	while (peek().isSpace()) {
		m_index++;
	}
}

int FilenameParser::indexOf(const QList<QChar> &chars, int max)
{
	bool escapeNext = false;

	int limit = max < 0 ? m_str.count() : qMin(max, m_str.count());
	for (int index = m_index; index < limit; ++index) {
		QChar c = m_str[index];

		// Don't return on escaped characters
		bool isEscape = c == '\\';
		if (isEscape && !escapeNext) {
			escapeNext = true;
		} else if (chars.contains(c) && !escapeNext) {
			return index;
		}

		// Clear escape character if unused
		if (!isEscape && escapeNext) {
			escapeNext = false;
		}
	}

	return -1;
}

QString FilenameParser::readUntil(const QList<QChar> &chars, bool allowEnd)
{
	int origPos = m_index;
	m_index = indexOf(chars);

	if (m_index < 0) {
		m_index = m_str.count();

		if (!allowEnd) {
			return QString();
		}

		return m_str.mid(origPos);
	}

	return m_str.mid(origPos, m_index - origPos);
}


FilenameNodeRoot *FilenameParser::parseRootNode()
{
	QList<FilenameNode*> exprs;

	while (!finished()) {
		exprs.append(parseExpr());
	}

	return new FilenameNodeRoot(exprs);
}

FilenameNode *FilenameParser::parseExpr(const QList<QChar> &addChars)
{
	QChar p = peek();

	if (p == '<') {
		return parseConditional();
	}
	if (p == '%') {
		return parseVariable();
	}

	QList<QChar> until = QList<QChar>{ '<', '%' } + addChars;
	QString txt = readUntil(until, true);

	return new FilenameNodeText(txt);
}

FilenameNodeVariable *FilenameParser::parseVariable()
{
	m_index++; // %

	QString name = readUntil({ ':', '%' });

	QMap<QString, QString> opts;
	while (peek() != '%') {
		m_index++; // : or ,

		QString opt = readUntil({ '=', ',', '%' });

		QString val;
		if (peek() == '=') {
			m_index++; // =
			val = readUntil({ ',', '%' });
		}

		opts.insert(opt, val);
	}

	m_index++; // %

	return new FilenameNodeVariable(name, opts);
}

FilenameNodeConditional *FilenameParser::parseConditional()
{
	m_index++; // <

	FilenameNodeCondition *condition = nullptr;
	FilenameNode *ifTrue = nullptr;
	FilenameNode *ifFalse = nullptr;

	// Legacy conditionals
	int endIndex = indexOf({ '>' });
	int sepIndex = indexOf({ '?' }, endIndex);
	if (sepIndex < 0) {
		QList<FilenameNode*> exprs;
		QList<FilenameNodeCondition*> conds;

		while (peek() != '>') {
			QList<QChar> stop { '>', '!', '"', '%' };
			if (!stop.contains(peek())) {
				exprs.append(parseExpr(stop));
			}

			if (peek() != '>') {
				auto cond = parseSingleCondition();
				conds.append(cond);
				exprs.append(cond);
			}
		}

		if (conds.isEmpty()) {
			throw std::runtime_error("No condition found in conditional");
		}

		condition = conds.takeFirst();
		while (!conds.isEmpty()) {
			condition = new FilenameNodeConditionOp(FilenameNodeConditionOp::And, condition, conds.takeFirst());
		}

		ifTrue = exprs.count() == 1
			? exprs.first()
			: new FilenameNodeRoot(exprs);
	} else {
		condition = parseConditionNode();
		if (peek() != '?') {
			delete condition;
			throw std::runtime_error("Expected '?' after condition");
		}
		m_index++; // ?

		ifTrue = parseExpr({ ':', '>' });
		if (peek() == ':') {
			m_index++; // :
			ifFalse = parseExpr({ '>' });
		}

		if (peek() != '>') {
			delete condition;
			delete ifTrue;
			delete ifFalse;
			throw std::runtime_error("Expected '>' at the end of contional");
		}
	}

	m_index++; // >

	return new FilenameNodeConditional(condition, ifTrue, ifFalse);
}

FilenameNodeCondition *FilenameParser::parseConditionNode()
{
	skipSpaces();

	FilenameNodeCondition *lhs;

	// Parenthesis
	QChar p = peek();
	if (p == '(') {
		m_index++; // (

		lhs = parseConditionNode();
		if (peek() != ')') {
			delete lhs;
			throw std::runtime_error("Expected ')' after condition in parenthesis");
		}

		m_index++; // )
	} else {
		lhs = parseSingleCondition();
	}

	QStack<QChar> opsStack;
	QStack<FilenameNodeCondition*> termStack;

	while (!finished()) {
		skipSpaces();

		QChar p = peek();
		QChar c = p == '"' || p == '%' ? '&' : p;
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

		if (p == c) {
			m_index++;
			skipSpaces();
		}

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

	skipSpaces();

	return term;
}

FilenameNodeCondition *FilenameParser::parseSingleCondition()
{
	QChar c = peek();

	if (c == '!') {
		return parseConditionInvert();
	}
	if (c == '%') {
		return parseConditionToken();
	}
	if (c == '"') {
		return parseConditionTag();
	}

	throw std::runtime_error("Expected '!', '%' or '\"' for condition");
}

FilenameNodeConditionInvert *FilenameParser::parseConditionInvert()
{
	m_index++; // !

	auto cond = parseSingleCondition();

	return new FilenameNodeConditionInvert(cond);
}

FilenameNodeConditionTag *FilenameParser::parseConditionTag()
{
	m_index++; // "

	QString tag = readUntil({ '"' });

	m_index++; // "

	return new FilenameNodeConditionTag(Tag(tag));
}

FilenameNodeConditionToken *FilenameParser::parseConditionToken()
{
	m_index++; // %

	QString token = readUntil({ '%' });

	m_index++; // %

	return new FilenameNodeConditionToken(token);
}
