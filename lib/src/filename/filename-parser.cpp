#include "filename/filename-parser.h"
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

QString FilenameParser::readUntil(const QList<QChar> &chars, bool allowEnd)
{
	int origPos = m_index;
	bool escapeNext = false;

	while (!finished()) {
		QChar c = peek();
		if (c == '\\' && !escapeNext) {
			escapeNext = true;
		} else if (chars.contains(c) && !escapeNext) {
			return m_str.mid(origPos, m_index - origPos);
		}
		m_index++;
	}

	if (!allowEnd) {
		return QString();
	}

	return m_str.mid(origPos);
}


FilenameNodeRoot *FilenameParser::parseRoot()
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
	if (false) {
		QList<FilenameNode*> exprs;
		bool invert = false;

		/*while (peek() != '>') {
			exprs.push(parseExpr({ '>', '!', '"', '%' }));

			if (peek() == '!') {
				invert = true;
				m_index++; // !
			}

			if (peek() == '"') {
				m_index++; // "

				QString cond = parseString(reader);

				cond.invert = invert;
				invert = false;

				exprs.push(cond);
			} else if (peek() == '%') {
				m_index++; // %
				const cond = parseVariable(reader);

				cond.invert = invert;
				invert = false;

				exprs.push(cond);
			}
		}*/
	} else {
		condition = parseCondition();
		if (peek() != '|') {
			return nullptr;
		}

		ifTrue = parseExpr({ '|', '>' });
		if (peek() == '|') {
			ifFalse = parseExpr({ '|', '>' });
		}

		if (peek() != '>') {
			return nullptr;
		}
	}

	m_index++; // >

	return new FilenameNodeConditional(condition, ifTrue, ifFalse);
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

	if (c == '!') {
		return parseConditionInvert();
	}
	if (c == '%') {
		return parseConditionToken();
	}
	if (c == '"') {
		return parseConditionTag();
	}

	return nullptr;
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
