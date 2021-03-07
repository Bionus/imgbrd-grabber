#include "tools/tag-list-loader.h"
#include <QEventLoop>
#include <QObject>
#include "models/api/api.h"
#include "models/profile.h"
#include "models/site.h"
#include "tags/tag.h"
#include "tags/tag-api.h"
#include "tags/tag-database.h"
#include "tags/tag-type-api.h"


TagListLoader::TagListLoader(Profile *profile, Site *site, int minTagCount, QObject *parent)
	: QObject(parent), m_profile(profile), m_site(site), m_minTagCount(minTagCount)
{}

bool TagListLoader::canLoadTags(Site *site)
{
	bool needTagTypes = site->tagDatabase()->tagTypes().isEmpty();
	bool hasApiForTagTypes = !getApisToLoadTagTypes(site).isEmpty();
	bool hasApiForTags = !getApisToLoadTags(site, needTagTypes && !hasApiForTagTypes).isEmpty();
	return hasApiForTags;
}


QList<Api*> TagListLoader::getApisToLoadTagTypes(Site *site)
{
	QList<Api*> apis;
	for (Api *a : site->getApis()) {
		if (a->canLoadTagTypes()) {
			apis.append(a);
		}
	}

	return apis;
}

QList<Api*> TagListLoader::getApisToLoadTags(Site *site, bool needTagTypes)
{
	QList<Api*> apis;
	for (Api *a : site->getApis()) {
		if (a->canLoadTags() && (!needTagTypes || !a->mustLoadTagTypes())) {
			apis.append(a);
		}
	}

	return apis;
}


void TagListLoader::start()
{
	m_error.clear();
	m_results.clear();
	m_cancelled = false;

	m_needTagTypes = m_site->tagDatabase()->tagTypes().isEmpty();
	QList<Api*> apisTypes = getApisToLoadTagTypes(m_site);

	// Load tag types first if necessary (and possible)
	if (m_needTagTypes && !apisTypes.isEmpty()) {
		loadTagTypes(apisTypes.first());
	} else {
		loadTags();
	}
}

void TagListLoader::loadTagTypes(Api *apiTypes)
{
	emit progress(tr("Loading tag types..."));

	QEventLoop loop;
	auto *tagTypeApi = new TagTypeApi(m_profile, m_site, apiTypes, this);
	connect(tagTypeApi, &TagTypeApi::finishedLoading, &loop, &QEventLoop::quit);
	tagTypeApi->load();
	loop.exec();

	auto tagTypes = tagTypeApi->tagTypes();
	if (tagTypes.isEmpty()) {
		m_error = tr("Error loading tag types.");
		emit finished();
		return;
	}

	m_site->tagDatabase()->setTagTypes(tagTypes);
	m_needTagTypes = false;

	loadTags();
}

void TagListLoader::loadTags()
{
	// Initialize the tag database
	m_site->tagDatabase()->load();

	// Get tag loading API
	QList<Api*> apis = getApisToLoadTags(m_site, m_needTagTypes);
	m_api = apis.first();

	m_oldCount = -1;
	m_currentPage = 1;

	emit progress("0 - 0");
	loadNextPage();
}

void TagListLoader::loadNextPage()
{
	m_currentTagApi = new TagApi(m_profile, m_site, m_api, m_currentPage, 500, "count", this);
	connect(m_currentTagApi, &TagApi::finishedLoading, this, &TagListLoader::tagsLoaded);
	m_currentTagApi->load();
}

void TagListLoader::tagsLoaded()
{
	if (m_cancelled) {
		return;
	}

	bool newTag = false;
	QList<Tag> tags = m_currentTagApi->tags();
	for (const auto &tag : tags) {
		if (tag.count() == 0 || tag.count() >= m_minTagCount) {
			m_results.append(tag);
			newTag = true;
		}
	}

	emit progress(QString("%1 - %2").arg(m_currentPage).arg(m_results.count()));

	m_currentTagApi->deleteLater();
	m_currentTagApi = nullptr;

	// As long as we keep finding new tags, we continue loading
	if (newTag) {
		m_currentPage++;
		loadNextPage();
		return;
	}

	// Otherwise, that means we finished
	m_site->tagDatabase()->setTags(m_results, !m_api->mustLoadTagTypes());
	m_site->tagDatabase()->save();

	emit finished();
}

void TagListLoader::cancel()
{
	m_cancelled = true;
}
