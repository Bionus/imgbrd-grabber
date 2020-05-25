#include "search/ast/search-node-op.h"
#include "search/ast/search-visitor.h"


SearchNodeOp::SearchNodeOp(Operator op, SearchNode *left, SearchNode *right)
	: op(op), left(left), right(right)
{}

SearchNodeOp::~SearchNodeOp()
{
	delete left;
	delete right;
}

void SearchNodeOp::accept(SearchVisitor &v) const
{
	v.visit(*this);
}
