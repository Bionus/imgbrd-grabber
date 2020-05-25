#ifndef SEARCH_FORMAT_H
#define SEARCH_FORMAT_H

#include <QString>


struct SearchFormatType
{
	QString separator;
	QString prefix;
};

class SearchFormat
{
	public:
		enum Precedence
		{
			And,
			Or,
		};

		SearchFormat() = default;
		SearchFormat(SearchFormatType andOp, SearchFormatType orOp, bool parenthesis, Precedence precedence);

		const SearchFormatType &andOp() const;
		const SearchFormatType &orOp() const;
		bool parenthesis() const;
		Precedence precedence() const;

	private:
		SearchFormatType m_andOp;
		SearchFormatType m_orOp;
		bool m_parenthesis;
		Precedence m_precedence;
};

#endif // SEARCH_FORMAT_H
