#include "search/search-parser.h"
#include <QStringList>
#include <utility>
#include "search/ast/search-node.h"
#include "search/ast/search-node-op.h"
#include "search/ast/search-node-tag.h"


SearchParser::SearchParser(QString str)
	: SearchParser(std::move(str), SearchFormat::Normalized(), TagNameFormat::Normalized())
{}

SearchParser::SearchParser(QString str, SearchFormat searchFormat, TagNameFormat tagNameFormat)
	: m_str(std::move(str)), m_searchFormat(std::move(searchFormat)), m_tagNameFormat(std::move(tagNameFormat))
{}


const QString &SearchParser::error() const
{
	return m_error;
}

const QString &SearchParser::str() const
{
	return m_str;
}


SearchNode *SearchParser::parse()
{
	// Empty searches simply return null
	if (m_str.isEmpty()) {
		return nullptr;
	}

	// TODO(Bionus): implement a proper parser
	QStringList parts = m_str.split(m_searchFormat.andOp().separator);

	// Single tag searches
	if (parts.count() == 1) {
		return makeTagNode(parts.first());
	}

	// Otherwise, loop and create as many Op nodes as needed
	SearchNode *right = makeTagNode(parts.takeLast());
	while (parts.count() > 0) {
		SearchNode *left = makeTagNode(parts.takeLast());
		right = new SearchNodeOp(SearchNodeOp::Operator::And, left, right);
	}
	return right;
}

SearchNodeTag *SearchParser::makeTagNode(const QString &tag) const
{
	return new SearchNodeTag(tag.split(m_tagNameFormat.wordSeparator()));
}
