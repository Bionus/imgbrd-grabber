#include "filename/filename-text-extraction-visitor.h"
#include <QList>
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-text.h"


QStringList FilenameTextExtractionVisitor::run(const FilenameNodeRoot &node)
{
	m_results.clear();

	node.accept(*this);

	return m_results;
}


void FilenameTextExtractionVisitor::visit(const FilenameNodeText &node)
{
	m_results.append(node.text);
}
