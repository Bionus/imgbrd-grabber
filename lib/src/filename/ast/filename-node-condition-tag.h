#ifndef FILENAME_NODE_CONDITION_TAG_H
#define FILENAME_NODE_CONDITION_TAG_H

#include "filename/ast/filename-node-condition.h"
#include "tags/tag.h"


class Filter;

struct FilenameNodeConditionTag : public FilenameNodeCondition
{
	Tag tag;
	Filter *filter;

	explicit FilenameNodeConditionTag(Tag tag);
	~FilenameNodeConditionTag() override;
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_CONDITION_TAG_H
