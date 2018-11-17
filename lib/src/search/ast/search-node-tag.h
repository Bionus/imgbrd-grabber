#ifndef SEARCH_NODE_TAG_H
#define SEARCH_NODE_TAG_H

#include "search/ast/search-node.h"
#include "tags/tag.h"


struct SearchNodeTag : public SearchNode
{
	Tag tag;

	SearchNodeTag(Tag tag);
	void accept(SearchVisitor &v) const override;
};

#endif // SEARCH_NODE_TAG_H
