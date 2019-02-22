#include "filename/ast/filename-node-condition-ignore.h"
#include "filename/ast/filename-visitor.h"


FilenameNodeConditionIgnore::FilenameNodeConditionIgnore(FilenameNodeCondition *node)
	: node(node)
{}

FilenameNodeConditionIgnore::~FilenameNodeConditionIgnore()
{
	delete node;
}

void FilenameNodeConditionIgnore::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
