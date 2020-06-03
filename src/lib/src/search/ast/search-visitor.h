#ifndef SEARCH_VISITOR_H
#define SEARCH_VISITOR_H

struct SearchNodeOp;
struct SearchNodeTag;


class SearchVisitor
{
	public:
		virtual ~SearchVisitor() = default;
		virtual void visit(const SearchNodeOp &node) = 0;
		virtual void visit(const SearchNodeTag &node) = 0;
};

#endif // SEARCH_VISITOR_H
