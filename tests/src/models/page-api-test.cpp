#include <QScopedPointer>
#include <QSettings>
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
	settings.setValue("auth/password", "a867ce3dbb1f52ccb763d4a1ff4bee5baaea37c1");
	settings.sync();

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

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

		REQUIRE(pageApi.url().toString() == QString("https://danbooru.donmai.us/posts.xml?limit=25&page=1&tags=test tag&login=user&password_hash=a867ce3dbb1f52ccb763d4a1ff4bee5baaea37c1"));
	}

	SECTION("ParseUrlAltPage")
	{
		Site *site = sites.first();

		QStringList tags = QStringList() << "test" << "tag";
		Page prevPage(profile, site, sites, tags, 1000);
		Page page(profile, site, sites, tags, 1001);
		PageApi pageApi(&page, profile, site, site->getApis().first(), tags, 1001);
		pageApi.setLastPage(&prevPage);

		REQUIRE(pageApi.url().toString() == QString("https://danbooru.donmai.us/posts.xml?limit=25&page=b0&tags=test tag&login=user&password_hash=a867ce3dbb1f52ccb763d4a1ff4bee5baaea37c1"));
	}
}
