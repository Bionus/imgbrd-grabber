#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-visitor.h"


FilenameNodeConditionInvert::FilenameNodeConditionInvert(FilenameNodeCondition *node)
	: node(node)
{}

FilenameNodeConditionInvert::~FilenameNodeConditionInvert()
{
	delete node;
}

void FilenameNodeConditionInvert::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
