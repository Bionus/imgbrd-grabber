#ifndef FILENAME_NODE_CONDITION_OP_H
#define FILENAME_NODE_CONDITION_OP_H

#include "filename/ast/filename-node-condition.h"


struct FilenameNodeConditionOp : public FilenameNodeCondition
{
	enum Operator
	{
		And,
		Or,
	};

	Operator op;
	FilenameNodeCondition *left;
	FilenameNodeCondition *right;

	FilenameNodeConditionOp(Operator op, FilenameNodeCondition *left, FilenameNodeCondition *right);
	~FilenameNodeConditionOp() override;
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_CONDITION_OP_H
