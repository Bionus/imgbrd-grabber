#include <QSignalSpy>
#include "custom-network-access-manager.h"
#include "models/api/api.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "tags/tag-type-api.h"
#include "catch.h"
#include "source-helpers.h"

TagTypeApi::LoadResult load(TagTypeApi *api)
{
	// Wait for downloader
	QSignalSpy spy(api, SIGNAL(finishedLoading(TagApiBase*, TagApiBase::LoadResult)));
	api->load(false);
	if (!spy.wait()) {
		return TagTypeApi::LoadResult::Error;
	}

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	TagTypeApi::LoadResult result = arguments.at(1).value<TagTypeApi::LoadResult>();

	return result;
}


TEST_CASE("TagTypeApi")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	Api *api = nullptr;
	for (Api *a : site->getApis()) {
		if (a->getName() == "Html") {
			api = a;
		}
	}


	SECTION("Basic")
	{
		TagTypeApi tagTypeApi(profile, site, api);
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.html");

		TagTypeApi::LoadResult result = load(&tagTypeApi);

		REQUIRE(result == TagTypeApi::LoadResult::Ok);
		REQUIRE(tagTypeApi.tagTypes().count() == 4);
		REQUIRE(tagTypeApi.tagTypes().at(1).id == 1);
		REQUIRE(tagTypeApi.tagTypes().at(1).name == "artist");
	}

	SECTION("NetworkError")
	{
		TagTypeApi tagTypeApi(profile, site, api);
		CustomNetworkAccessManager::NextFiles.enqueue("404");

		TagTypeApi::LoadResult result = load(&tagTypeApi);

		REQUIRE(result == TagTypeApi::LoadResult::Error);
		REQUIRE(tagTypeApi.tagTypes().count() == 0);
	}

	SECTION("ParseError")
	{
		TagTypeApi tagTypeApi(profile, site, api);
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.xml");

		TagTypeApi::LoadResult result = load(&tagTypeApi);

		REQUIRE(result == TagTypeApi::LoadResult::Error);
		REQUIRE(tagTypeApi.tagTypes().count() == 0);
	}

	SECTION("DoubleLoad")
	{
		TagTypeApi tagTypeApi(profile, site, api);
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.html");
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.html");

		load(&tagTypeApi);
		TagTypeApi::LoadResult result = load(&tagTypeApi);

		REQUIRE(result == TagTypeApi::LoadResult::Ok);
	}

	SECTION("Redirect")
	{
		TagTypeApi tagTypeApi(profile, site, api);
		CustomNetworkAccessManager::NextFiles.enqueue("redirect");
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.html");

		TagTypeApi::LoadResult result = load(&tagTypeApi);

		REQUIRE(result == TagTypeApi::LoadResult::Ok);
	}

	SECTION("Abort")
	{
		TagTypeApi tagTypeApi(profile, site, api);

		QSignalSpy spy(&tagTypeApi, SIGNAL(finishedLoading(TagApiBase*, TagApiBase::LoadResult)));
		tagTypeApi.load(false);
		tagTypeApi.abort();
		REQUIRE(!spy.wait(1000));
	}
}
