#include "filename/ast/filename-node-conditional.h"
#include "filename/ast/filename-visitor.h"


FilenameNodeConditional::FilenameNodeConditional(FilenameNodeCondition *condition, FilenameNode *ifTrue, FilenameNode *ifFalse)
	: condition(condition), ifTrue(ifTrue), ifFalse(ifFalse)
{}

FilenameNodeConditional::~FilenameNodeConditional()
{
	delete condition;
	delete ifTrue;
	delete ifFalse;
}

void FilenameNodeConditional::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
