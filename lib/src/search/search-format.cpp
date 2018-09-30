#include "search/search-format.h"


SearchFormat::SearchFormat(SearchFormatType andOp, SearchFormatType orOp, bool parenthesis, Precedence precedence)
	: m_andOp(std::move(andOp)), m_orOp(std::move(orOp)), m_parenthesis(parenthesis), m_precedence(precedence)
{}


const SearchFormatType &SearchFormat::andOp() const
{
	return m_andOp;
}

const SearchFormatType &SearchFormat::orOp() const
{
	return m_orOp;
}

bool SearchFormat::parenthesis() const
{
	return m_parenthesis;
}

SearchFormat::Precedence SearchFormat::precedence() const
{
	return m_precedence;
}
