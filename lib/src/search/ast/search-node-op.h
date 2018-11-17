#ifndef SEARCH_NODE_OP_H
#define SEARCH_NODE_OP_H

#include "search/ast/search-node.h"


struct SearchNodeOp : public SearchNode
{
	enum Operator
	{
		And,
		Or,
	};

	Operator op;
	SearchNode *left;
	SearchNode *right;

	SearchNodeOp(Operator op, SearchNode *left, SearchNode *right);
	~SearchNodeOp() override;
	void accept(SearchVisitor &v) const override;
};

#endif // SEARCH_NODE_OP_H
