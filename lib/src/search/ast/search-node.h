#ifndef SEARCH_NODE_H
#define SEARCH_NODE_H

class SearchVisitor;


struct SearchNode
{
	virtual ~SearchNode() = default;
	virtual void accept(SearchVisitor &v) const = 0;
};

#endif // SEARCH_NODE_H
