#include <QSignalSpy>
#include "custom-network-access-manager.h"
#include "models/api/api.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "tags/tag.h"
#include "tags/tag-api.h"
#include "catch.h"
#include "source-helpers.h"


TagApi::LoadResult load(TagApi *api)
{
	// Wait for downloader
	QSignalSpy spy(api, SIGNAL(finishedLoading(TagApiBase*, TagApiBase::LoadResult)));
	api->load(false);
	if (!spy.wait()) {
		return TagApi::LoadResult::Error;
	}

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	TagApi::LoadResult result = arguments.at(1).value<TagApi::LoadResult>();

	return result;
}


TEST_CASE("TagApi")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	Api *api = nullptr;
	for (Api *a : site->getApis()) {
		if (a->getName() == "Xml") {
			api = a;
		}
	}


	SECTION("Basic")
	{
		TagApi tagApi(profile, site, api, 1, 100);
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.xml");

		TagApi::LoadResult result = load(&tagApi);

		REQUIRE(result == TagApi::LoadResult::Ok);
		REQUIRE(tagApi.tags().count() == 100);
		REQUIRE(tagApi.tags().at(1).text() == QString("walkr"));
		REQUIRE(tagApi.tags().at(1).type().name() == QString("copyright"));
	}

	SECTION("NetworkError")
	{
		TagApi tagApi(profile, site, api, 1, 100);
		CustomNetworkAccessManager::NextFiles.enqueue("404");

		TagApi::LoadResult result = load(&tagApi);

		REQUIRE(result == TagApi::LoadResult::Error);
		REQUIRE(tagApi.tags().count() == 0);
	}

	SECTION("ParseError")
	{
		TagApi tagApi(profile, site, api, 1, 100);
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.html");

		TagApi::LoadResult result = load(&tagApi);

		REQUIRE(result == TagApi::LoadResult::Error);
		REQUIRE(tagApi.tags().count() == 0);
	}

	SECTION("DoubleLoad")
	{
		TagApi tagApi(profile, site, api, 1, 100);
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.xml");
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.xml");

		load(&tagApi);
		TagApi::LoadResult result = load(&tagApi);

		REQUIRE(result == TagApi::LoadResult::Ok);
	}

	SECTION("Redirect")
	{
		TagApi tagApi(profile, site, api, 1, 100);
		CustomNetworkAccessManager::NextFiles.enqueue("redirect");
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.xml");

		TagApi::LoadResult result = load(&tagApi);

		REQUIRE(result == TagApi::LoadResult::Ok);
	}

	SECTION("Abort")
	{
		TagApi tagApi(profile, site, api, 1, 100);

		QSignalSpy spy(&tagApi, SIGNAL(finishedLoading(TagApiBase*, TagApiBase::LoadResult)));
		tagApi.load(false);
		tagApi.abort();
		REQUIRE(!spy.wait(1000));
	}
}
