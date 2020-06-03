#include "filename/filename-print-visitor.h"
#include "filename/ast/filename-node-condition-ignore.h"
#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-node-condition-javascript.h"
#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/ast/filename-node-conditional.h"
#include "filename/ast/filename-node-javascript.h"
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-text.h"
#include "filename/ast/filename-node-variable.h"


QString FilenamePrintVisitor::run(const FilenameNodeRoot &node)
{
	m_result.clear();

	node.accept(*this);

	return m_result;
}


void FilenamePrintVisitor::visit(const FilenameNodeConditional &node)
{
	m_result += "Conditional(";

	node.condition->accept(*this);

	m_result += ";";
	node.ifTrue->accept(*this);

	if (node.ifFalse != nullptr) {
		m_result += ";";
		node.ifFalse->accept(*this);
	}

	m_result += ")";
}

void FilenamePrintVisitor::visit(const FilenameNodeConditionIgnore &node)
{
	m_result += "ConditionIgnore(";
	node.node->accept(*this);
	m_result += ")";
}

void FilenamePrintVisitor::visit(const FilenameNodeConditionInvert &node)
{
	m_result += "ConditionInvert(";
	node.node->accept(*this);
	m_result += ")";
}

void FilenamePrintVisitor::visit(const FilenameNodeConditionJavaScript &node)
{
	m_result += "ConditionJavaScript('";
	m_result += node.script;
	m_result += "')";
}

void FilenamePrintVisitor::visit(const FilenameNodeConditionOp &node)
{
	m_result += "ConditionOp(";

	node.left->accept(*this);

	m_result += ";";
	m_result += node.op == FilenameNodeConditionOp::Operator::And ? "And" : "Or";
	m_result += ";";

	node.right->accept(*this);

	m_result += ")";
}

void FilenamePrintVisitor::visit(const FilenameNodeConditionTag &node)
{
	m_result += "ConditionTag('";
	m_result += node.tag.text();
	m_result += "')";
}

void FilenamePrintVisitor::visit(const FilenameNodeConditionToken &node)
{
	m_result += "ConditionToken('";
	m_result += node.token;
	m_result += "')";
}

void FilenamePrintVisitor::visit(const FilenameNodeJavaScript &node)
{
	m_result += "JavaScript('";
	m_result += node.script;
	m_result += "')";
}

void FilenamePrintVisitor::visit(const FilenameNodeRoot &node)
{
	m_result += "Root(";

	int i = 0;
	for (auto expr : node.exprs) {
		if (i++ > 0) {
			m_result += ";";
		}
		expr->accept(*this);
	}

	m_result += ")";
}

void FilenamePrintVisitor::visit(const FilenameNodeText &node)
{
	m_result += "Text('";
	m_result += node.text;
	m_result += "')";
}

void FilenamePrintVisitor::visit(const FilenameNodeVariable &node)
{
	m_result += "Variable('";
	m_result += node.name;
	m_result += "'";

	if (!node.opts.isEmpty()) {
		m_result += ";";

		int i = 0;
		for (const auto &opt : node.opts.keys()) {
			if (i++ > 0) {
				m_result += ",";
			}
			m_result += opt;
			if (!node.opts[opt].isEmpty()) {
				m_result += "=" + node.opts[opt];
			}
		}
	}

	m_result += ")";
}
