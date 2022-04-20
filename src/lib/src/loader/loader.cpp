#include "loader.h"
#include "loader-query.h"


Loader::Loader(Profile *profile, Site *site)
	: m_profile(profile), m_site(site)
{}

LoaderQuery Loader::search(const QMap<QString, QVariant> &options) const
{
	return LoaderQuery(m_profile, m_site, options);
}
