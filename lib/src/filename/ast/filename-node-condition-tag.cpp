#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-visitor.h"


FilenameNodeConditionTag::FilenameNodeConditionTag(Tag tag)
	: tag(std::move(tag))
{}

void FilenameNodeConditionTag::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
