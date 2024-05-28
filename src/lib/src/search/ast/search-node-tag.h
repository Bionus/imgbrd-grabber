#ifndef SEARCH_NODE_TAG_H
#define SEARCH_NODE_TAG_H

#include "search/ast/search-node.h"
#include "tags/tag.h"


class SearchVisitor;

struct SearchNodeTag : public SearchNode
{
	QStringList tag;

	explicit SearchNodeTag(const Tag &tag);
	explicit SearchNodeTag(QStringList words);

	void accept(SearchVisitor &v) const override;
};

#endif // SEARCH_NODE_TAG_H
