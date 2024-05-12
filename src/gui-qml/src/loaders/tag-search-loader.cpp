#include "tag-search-loader.h"
#include <QString>


TagSearchLoader::TagSearchLoader(QObject *parent)
	: SearchLoader(parent)
{}


const QString &TagSearchLoader::query() const
{
	return m_query;
}

void TagSearchLoader::setQuery(const QString &query)
{
	m_query = query;
	emit queryChanged();
}


void TagSearchLoader::load()
{
	if (!m_endpoint.isEmpty()) {
		const QMap<QString, QVariant> input {{ "search", m_query }};
		search(SearchQuery(m_endpoint, input));
	} else {
		search(m_query.split(" ", Qt::SkipEmptyParts));
	}
}
