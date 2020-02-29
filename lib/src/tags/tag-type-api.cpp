#include "tags/tag-type-api.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/site.h"


TagTypeApi::TagTypeApi(Profile *profile, Site *site, Api *api, QObject *parent)
	: TagApiBase(profile, site, api, parent)
{
	const QString url = api->tagTypesUrl(site).url;
	setUrl(site->fixUrl(url));
}

void TagTypeApi::parse(const QString &source, int statusCode, Site *site)
{
	Q_UNUSED(site);

	ParsedTagTypes ret = m_api->parseTagTypes(source, statusCode, m_site);
	if (!ret.error.isEmpty()) {
		log(QStringLiteral("[%1][%2] %3").arg(m_site->url(), m_api->getName(), ret.error), Logger::Warning);
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	m_tagTypes.clear();
	m_tagTypes.append(ret.types);

	emit finishedLoading(this, LoadResult::Ok);
}

const QList<TagTypeWithId> &TagTypeApi::tagTypes() const
{
	return m_tagTypes;
}
