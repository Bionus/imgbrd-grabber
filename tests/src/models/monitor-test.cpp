#include <QJsonObject>
#include <QScopedPointer>
#include "models/monitor.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "catch.h"
#include "source-helpers.h"


TEST_CASE("Monitor")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	const QScopedPointer<Profile> profile(makeProfile());
	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);
	QList<Site*> sites { site };

	SECTION("Site")
	{
		Monitor monitor(sites, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);

		REQUIRE(monitor.sites() == sites);
	}

	SECTION("Interval")
	{
		Monitor monitor(sites, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);

		REQUIRE(monitor.interval() == 60);
	}

	SECTION("LastCheck")
	{
		QDateTime before(QDate(2016, 7, 2), QTime(16, 35, 12));
		QDateTime after(QDate(2018, 7, 2), QTime(16, 35, 12));

		Monitor monitor(sites, 60, before, false, "", "", 12, true);

		REQUIRE(monitor.lastCheck() == before);
		monitor.setLastCheck(after);
		REQUIRE(monitor.lastCheck() == after);
	}

	SECTION("Cumulated")
	{
		Monitor monitor(sites, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);

		REQUIRE(monitor.cumulated() == 12);
		REQUIRE(monitor.preciseCumulated() == true);
		monitor.setCumulated(20, false);
		REQUIRE(monitor.cumulated() == 20);
		REQUIRE(monitor.preciseCumulated() == false);
	}

	SECTION("Serialization")
	{
		Monitor original(sites, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);

		QJsonObject json;
		original.toJson(json);

		Monitor dest = Monitor::fromJson(json, profile.data());

		REQUIRE(dest.sites() == original.sites());
		REQUIRE(dest.interval() == original.interval());
		REQUIRE(dest.lastCheck() == original.lastCheck());
		REQUIRE(dest.cumulated() == original.cumulated());
		REQUIRE(dest.preciseCumulated() == original.preciseCumulated());
	}

	SECTION("Compare")
	{
		Monitor a(sites, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);
		Monitor b(sites, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);
		Monitor c(sites, 120, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);

		REQUIRE(a == b);
		REQUIRE(b == a);
		REQUIRE(a != c);
		REQUIRE(b != c);
		REQUIRE(c == c);
	}
}
