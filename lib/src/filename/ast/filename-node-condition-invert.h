#ifndef FILENAME_NODE_CONDITION_INVERT_H
#define FILENAME_NODE_CONDITION_INVERT_H

#include "filename/ast/filename-node-condition.h"


struct FilenameNodeConditionInvert : public FilenameNodeCondition
{
	FilenameNodeCondition *node;

	explicit FilenameNodeConditionInvert(FilenameNodeCondition *node);
	~FilenameNodeConditionInvert() override;
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_CONDITION_INVERT_H
