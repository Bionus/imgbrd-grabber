#include <QPointer>
#include <QSignalSpy>
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "catch.h"
#include "source-helpers.h"


TEST_CASE("Page")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	setupSource("Gelbooru (0.2)");
	setupSite("Gelbooru (0.2)", "gelbooru.com");

	auto profile = QPointer<Profile>(makeProfile());
	QList<Site*> sites { profile->getSites().value("danbooru.donmai.us") };
	Site *site = profile->getSites().value("gelbooru.com");

	SECTION("IncompatibleModifiers")
	{
		Page page(profile, site, sites, QStringList() << "test" << "status:deleted");

		REQUIRE(page.search().count() == 1);
		REQUIRE(page.search().first() == QString("test"));
	}

	SECTION("LoadAbort")
	{
		Page page(profile, site, sites, QStringList() << "test" << "status:deleted");

		QSignalSpy spy(&page, SIGNAL(finishedLoading(Page*)));
		page.load();
		page.abort();
		REQUIRE(!spy.wait(1000));
	}

	SECTION("LoadTagsAbort")
	{
		Page page(profile, site, sites, QStringList() << "test" << "status:deleted");

		QSignalSpy spy(&page, SIGNAL(finishedLoadingTags(Page*)));
		page.loadTags();
		page.abortTags();
		REQUIRE(!spy.wait(1000));
	}
}
