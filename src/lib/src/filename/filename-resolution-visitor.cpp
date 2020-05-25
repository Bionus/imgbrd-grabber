#include "filename/filename-resolution-visitor.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-variable.h"


QSet<QString> FilenameResolutionVisitor::run(const FilenameNodeRoot &node)
{
	m_results.clear();

	node.accept(*this);

	return m_results;
}


void FilenameResolutionVisitor::visit(const FilenameNodeConditionToken &node)
{
	m_results.insert(node.token);
}

void FilenameResolutionVisitor::visit(const FilenameNodeVariable &node)
{
	m_results.insert(node.name);
}
