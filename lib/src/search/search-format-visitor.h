#ifndef SEARCH_FORMAT_VISITOR_H
#define SEARCH_FORMAT_VISITOR_H

#include "search/ast/search-visitor.h"
#include "search/search-format.h"


struct SearchNode;

class SearchFormatVisitor : public SearchVisitor
{
	public:
		explicit SearchFormatVisitor(SearchFormat searchFormat);
		QString run(const SearchNode &node);
		QString error() const;

		void visit(const SearchNodeOp &node) override;
		void visit(const SearchNodeTag &node) override;

	private:
		SearchFormat m_searchFormat;

		QString m_result;
		QString m_error;
		bool m_inPrecedent = false;
};

#endif // SEARCH_FORMAT_VISITOR_H
