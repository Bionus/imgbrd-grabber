#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-visitor.h"


FilenameNodeConditionOp::FilenameNodeConditionOp(Operator op, FilenameNodeCondition *left, FilenameNodeCondition *right)
	: op(op), left(left), right(right)
{}

FilenameNodeConditionOp::~FilenameNodeConditionOp()
{
	delete left;
	delete right;
}

void FilenameNodeConditionOp::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
