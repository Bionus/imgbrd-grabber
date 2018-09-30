#include "search/ast/search-node-tag.h"
#include "search/ast/search-visitor.h"


SearchNodeTag::SearchNodeTag(Tag tag)
	: tag(std::move(tag))
{}

void SearchNodeTag::accept(SearchVisitor &v) const
{
	v.visit(*this);
}
