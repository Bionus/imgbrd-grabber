#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-visitor.h"
#include "models/filtering/filter.h"
#include "models/filtering/filter-factory.h"


FilenameNodeConditionTag::FilenameNodeConditionTag(Tag tag)
	: tag(std::move(tag))
{
	filter = FilterFactory::build(this->tag.text());
}

FilenameNodeConditionTag::~FilenameNodeConditionTag()
{
	delete filter;
}

void FilenameNodeConditionTag::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
