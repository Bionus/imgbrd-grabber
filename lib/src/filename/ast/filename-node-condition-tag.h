#ifndef FILENAME_NODE_CONDITION_TAG_H
#define FILENAME_NODE_CONDITION_TAG_H

#include "filename/ast/filename-node-condition.h"
#include "tags/tag.h"


struct FilenameNodeConditionTag : public FilenameNodeCondition
{
	Tag tag;

	explicit FilenameNodeConditionTag(Tag tag);
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_CONDITION_TAG_H
