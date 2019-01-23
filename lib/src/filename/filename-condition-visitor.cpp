#include "filename/filename-condition-visitor.h"
#include "filename/ast/filename-node-condition.h"
#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "functions.h"
#include "loader/token.h"


FilenameConditionVisitor::FilenameConditionVisitor(QMap<QString, Token> tokens)
	: m_tokens(std::move(tokens))
{
	if (m_tokens.contains("allos")) {
		m_tags = m_tokens["allos"].value().toStringList();
	}
}

bool FilenameConditionVisitor::run(const FilenameNodeCondition &node)
{
	m_result = true;

	node.accept(*this);

	return m_result;
}


void FilenameConditionVisitor::visit(const FilenameNodeConditionInvert &node)
{
	node.node->accept(*this);

	m_result = !m_result;
}

void FilenameConditionVisitor::visit(const FilenameNodeConditionOp &node)
{
	node.left->accept(*this);

	// No need to evaluate the right operand in all cases
	if ((!m_result && node.op == FilenameNodeConditionOp::And)
			|| (m_result && node.op == FilenameNodeConditionOp::Or)) {
		return;
	}

	node.right->accept(*this);
}

void FilenameConditionVisitor::visit(const FilenameNodeConditionTag &node)
{
	m_result = m_tags.contains(node.tag.text());
}

void FilenameConditionVisitor::visit(const FilenameNodeConditionToken &node)
{
	m_result = m_tokens.contains(node.token) && !isVariantEmpty(m_tokens[node.token].value());
}
