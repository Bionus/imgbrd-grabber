#ifndef FILENAME_NODE_CONDITION_IGNORE_H
#define FILENAME_NODE_CONDITION_IGNORE_H

#include "filename/ast/filename-node-condition.h"


struct FilenameNodeConditionIgnore : public FilenameNodeCondition
{
	FilenameNodeCondition *node;

	explicit FilenameNodeConditionIgnore(FilenameNodeCondition *node);
	~FilenameNodeConditionIgnore() override;
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_CONDITION_IGNORE_H
