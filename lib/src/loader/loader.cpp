#include "loader.h"
#include "loader-query.h"


Loader::Loader(Site *site)
	: m_site(site)
{}

LoaderQuery Loader::search(QMap<QString, QVariant> options) const
{
	return LoaderQuery(m_site, options);
}
