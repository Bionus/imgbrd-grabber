#include <QScopedPointer>
#include <QSettings>
#include <QSignalSpy>
#include "custom-network-access-manager.h"
#include "models/page.h"
#include "models/page-api.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "tags/tag.h"
#include "catch.h"
#include "source-helpers.h"


TEST_CASE("PageApi")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	setupSource("Gelbooru (0.2)");
	setupSite("Gelbooru (0.2)", "gelbooru.com");

	QString path = "tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini";
	QSettings settings(path, QSettings::IniFormat);
	settings.setValue("auth/pseudo", "user");
	settings.setValue("auth/apiKey", "test-api-key");
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_max_retries", 2);
	settings.sync();

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto *profile = pProfile.data();

	QList<Site*> sites { profile->getSites().value("danbooru.donmai.us") };
	REQUIRE(sites[0] != nullptr);

	SECTION("ParseUrlBasic")
	{
		Site *site = profile->getSites().value("gelbooru.com");
		REQUIRE(site != nullptr);

		QStringList tags = QStringList() << "test" << "tag";
		Page page(profile, site, sites, tags);
		PageApi pageApi(&page, profile, site, site->getApis().first(), tags);

		REQUIRE(pageApi.url().toString() == QString("https://gelbooru.com/index.php?page=dapi&s=post&q=index&limit=25&pid=0&tags=test tag"));
	}

	SECTION("ParseUrlLogin")
	{
		Site *site = sites.first();

		QStringList tags = QStringList() << "test" << "tag";
		Page page(profile, site, sites, tags);
		PageApi pageApi(&page, profile, site, site->getApis().first(), tags);

		REQUIRE(pageApi.url().toString() == QString("https://danbooru.donmai.us/posts.xml?limit=25&page=1&tags=test tag&login=user&api_key=test-api-key"));
	}

	SECTION("ParseUrlAltPage")
	{
		Site *site = sites.first();

		QStringList tags = QStringList() << "test" << "tag";
		Page prevPage(profile, site, sites, tags, 1000);
		Page page(profile, site, sites, tags, 1001);
		PageApi pageApi(&page, profile, site, site->getApis().first(), tags, 1001);
		pageApi.setLastPage(prevPage.pageInformation());

		REQUIRE(pageApi.url().toString() == QString("https://danbooru.donmai.us/posts.xml?limit=25&page=b0&tags=test tag&login=user&api_key=test-api-key"));
	}

	SECTION("RateLimitRetriesAreBounded")
	{
		Site *site = sites.first();
		const QString statusCode = GENERATE(QString("429"), QString("503"), QString("509"));
		CustomNetworkAccessManager::NextFiles.enqueue(statusCode);
		CustomNetworkAccessManager::NextFiles.enqueue(statusCode);
		CustomNetworkAccessManager::NextFiles.enqueue(statusCode);

		QStringList tags = QStringList() << "test";
		Page page(profile, site, sites, tags);
		PageApi pageApi(&page, profile, site, site->getApis().first(), tags);
		QSignalSpy spy(&pageApi, SIGNAL(finishedLoading(PageApi*, PageApi::LoadResult)));

		pageApi.load();
		REQUIRE(spy.wait());

		const QList<QVariant> arguments = spy.takeFirst();
		REQUIRE(arguments.at(1).value<PageApi::LoadResult>() == PageApi::LoadResult::Error);
		REQUIRE(pageApi.errors().contains("Rate limit reached after 2 retries (HTTP " + statusCode + ")."));
		REQUIRE(CustomNetworkAccessManager::NextFiles.isEmpty());
	}
}
