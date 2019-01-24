#include "filename/ast/filename-node-text.h"
#include "filename/ast/filename-visitor.h"


FilenameNodeText::FilenameNodeText(QString text)
	: text(std::move(text))
{}

void FilenameNodeText::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
