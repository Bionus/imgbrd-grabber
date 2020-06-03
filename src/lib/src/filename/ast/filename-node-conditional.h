#ifndef FILENAME_NODE_CONDITIONAL_H
#define FILENAME_NODE_CONDITIONAL_H

#include "filename/ast/filename-node.h"


struct FilenameNodeCondition;

struct FilenameNodeConditional : public FilenameNode
{
	FilenameNodeCondition *condition;
	FilenameNode *ifTrue;
	FilenameNode *ifFalse;

	FilenameNodeConditional(FilenameNodeCondition *condition, FilenameNode *ifTrue, FilenameNode *ifFalse);
	~FilenameNodeConditional() override;
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_CONDITIONAL_H
