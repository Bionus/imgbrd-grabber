#include "models/search-query/tag-search-query.h"
#include <utility>


TagSearchQuery::TagSearchQuery(QStringList tags)
	: m_tags(std::move(tags))
{}

QStringList TagSearchQuery::tags() const
{
	return m_tags;
}
