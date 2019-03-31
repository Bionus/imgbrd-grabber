#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-visitor.h"


FilenameNodeRoot::FilenameNodeRoot(QList<FilenameNode*> exprs)
	: exprs(std::move(exprs))
{}

void FilenameNodeRoot::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
