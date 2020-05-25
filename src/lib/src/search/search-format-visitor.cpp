#include "search/search-format-visitor.h"
#include <utility>
#include "search/ast/search-node-op.h"
#include "search/ast/search-node-tag.h"
#include "search/ast/search-node.h"


SearchFormatVisitor::SearchFormatVisitor(SearchFormat searchFormat)
	: m_searchFormat(std::move(searchFormat))
{}

QString SearchFormatVisitor::run(const SearchNode &node)
{
	m_result.clear();
	m_error.clear();
	m_inPrecedent = false;

	node.accept(*this);

	return m_error.isEmpty() ? m_result : QString();
}

QString SearchFormatVisitor::error() const
{
	return m_error;
}


void SearchFormatVisitor::visit(const SearchNodeOp &node)
{
	const bool isPrecedent = (node.op == SearchNodeOp::And && m_searchFormat.precedence() == SearchFormat::And)
		|| (node.op == SearchNodeOp::Or && m_searchFormat.precedence() == SearchFormat::Or);

	const bool needParen = m_inPrecedent && !isPrecedent;
	if (needParen && !m_searchFormat.parenthesis()) {
		m_error = QStringLiteral("A parenthesis is required but the format does not support it");
		return;
	}

	if (isPrecedent) {
		m_inPrecedent = true;
	}

	const SearchFormatType &format = node.op == SearchNodeOp::And
		? m_searchFormat.andOp()
		: m_searchFormat.orOp();

	if (needParen) {
		m_result.append("(");
	}

	m_result.append(format.prefix);
	node.left->accept(*this);

	m_result.append(format.separator);

	m_result.append(format.prefix);
	node.right->accept(*this);

	if (needParen) {
		m_result.append(")");
	}
}

void SearchFormatVisitor::visit(const SearchNodeTag &node)
{
	m_result.append(node.tag.text());
}
