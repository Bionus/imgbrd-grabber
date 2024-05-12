#include "search/ast/search-node-tag.h"
#include <utility>
#include "search/ast/search-visitor.h"


SearchNodeTag::SearchNodeTag(const Tag &tag)
	: SearchNodeTag(tag.text().split(' '))
{}

SearchNodeTag::SearchNodeTag(QStringList words)
	: tag(std::move(words))
{}


void SearchNodeTag::accept(SearchVisitor &v) const
{
	v.visit(*this);
}
