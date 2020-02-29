#include "tags/tag-api.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/site.h"


TagApi::TagApi(Profile *profile, Site *site, Api *api, int page, int limit, QObject *parent)
	: TagApiBase(profile, site, api, parent), m_page(page), m_limit(limit)
{
	const QString url = api->tagsUrl(page, limit, site).url;
	setUrl(site->fixUrl(url));
}

void TagApi::parse(const QString &source, int statusCode, Site *site)
{
	Q_UNUSED(site);

	ParsedTags ret = m_api->parseTags(source, statusCode, m_site);
	if (!ret.error.isEmpty()) {
		log(QStringLiteral("[%1][%2] %3").arg(m_site->url(), m_api->getName(), ret.error), Logger::Warning);
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	m_tags.clear();
	m_tags.append(ret.tags);

	emit finishedLoading(this, LoadResult::Ok);
}

const QList<Tag> &TagApi::tags() const
{
	return m_tags;
}
