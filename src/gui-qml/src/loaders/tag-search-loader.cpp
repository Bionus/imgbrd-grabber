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
	search(m_query.split(' '));
}
