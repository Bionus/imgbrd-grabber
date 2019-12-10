#include <QScopedPointer>
#include "custom-network-access-manager.h"
#include "models/profile.h"
#include "models/source.h"
#include "models/source-guesser.h"
#include "catch.h"
#include "source-helpers.h"


TEST_CASE("SourceGuesser")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	const QScopedPointer<Profile> profile(makeProfile());

	SECTION("NotFound")
	{
		QList<Source*> sources;
		sources.append(profile->getSources().value("Danbooru"));

		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/homepage.html");

		SourceGuesser guesser("https://danbooru.donmai.us", sources);
		Source *source = guesser.start();

		REQUIRE(source == nullptr);
	}

	SECTION("NetworkError")
	{
		QList<Source*> sources;
		sources.append(profile->getSources().value("Danbooru"));

		CustomNetworkAccessManager::NextFiles.enqueue("404");

		SourceGuesser guesser("http://behoimi.org", sources);
		Source *source = guesser.start();

		REQUIRE(source == nullptr);
	}

	SECTION("Danbooru1")
	{
		QList<Source*> sources;
		sources.append(profile->getSources().value("Danbooru"));

		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/behoimi.org/homepage.html");

		SourceGuesser guesser("http://behoimi.org", sources);
		Source *source = guesser.start();

		REQUIRE(source != nullptr);
		REQUIRE(source->getName() == QString("Danbooru"));
	}

	SECTION("Danbooru2")
	{
		QList<Source*> sources;
		sources.append(profile->getSources().value("Danbooru (2.0)"));

		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/homepage.html");

		SourceGuesser guesser("https://danbooru.donmai.us", sources);
		Source *source = guesser.start();

		REQUIRE(source != nullptr);
		REQUIRE(source->getName() == QString("Danbooru (2.0)"));
	}
}
