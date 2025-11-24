#include "models/history/history.h"
#include <QFile>
#include <QThread>
#include "catch.h"
#include "models/profile.h"
#include "source-helpers.h"


TEST_CASE("History")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");
	const QScopedPointer<Profile> profile(makeProfile());
	Site *site = profile->getSites().value("danbooru.donmai.us");

	const QString filename = "test.txt";

	SECTION("Write and load")
	{
		SECTION("Empty")
		{
			History writer(filename, profile.data());
			REQUIRE(writer.save());

			History loader(filename, profile.data());
			REQUIRE(loader.load());

			REQUIRE(loader.entries().isEmpty());
			REQUIRE(QFile::remove(filename));
		}

		SECTION("Values")
		{
			History writer(filename, profile.data());
			writer.addQuery(SearchQuery({"search_1"}), {site});
			writer.addQuery(SearchQuery({"search_2"}), {site});
			writer.addQuery(SearchQuery({"search_3"}), {site});
			REQUIRE(writer.save());

			History loader(filename, profile.data());
			REQUIRE(loader.load());

			REQUIRE(loader.entries().count() == 3);
			REQUIRE(loader.entries()[0]->query.tags == QStringList("search_1"));
			REQUIRE(loader.entries()[1]->query.tags == QStringList("search_2"));
			REQUIRE(loader.entries()[2]->query.tags == QStringList("search_3"));

			REQUIRE(QFile::remove(filename));
		}
	}

	SECTION("Clear")
	{
		History history(filename, profile.data());
		history.addQuery(SearchQuery({"search_1"}), {site});
		REQUIRE(history.entries().count() == 1);

		history.clear();
		REQUIRE(history.entries().isEmpty());
	}

	SECTION("Add entries")
	{
		// New entry
		History history(filename, profile.data());
		history.addQuery(SearchQuery({"before"}), {site});
		history.addQuery(SearchQuery({"search"}), {site});

		QThread::msleep(1);
		const QDateTime now = QDateTime::currentDateTimeUtc();
		QThread::msleep(1);

		// The query should be at the top of the list: "before < search"
		REQUIRE(history.entries().count() == 2);
		REQUIRE(history.entries()[1]->query.tags == QStringList("search"));
		REQUIRE(history.entries()[1]->date < now);

		// Add a new entry then replace the existing one
		history.addQuery(SearchQuery({"after"}), {site});
		history.addQuery(SearchQuery({"search"}), {site});

		// The query should be back to the top of the list: "before < after < search"
		REQUIRE(history.entries().count() == 3);
		REQUIRE(history.entries()[2]->query.tags == QStringList("search"));
		REQUIRE(history.entries()[2]->date > now);
	}

	SECTION("Add entries")
	{
		History history(filename, profile.data());
		history.addQuery(SearchQuery({"search_1"}), {site});
		history.addQuery(SearchQuery({"search_2"}), {site});
		history.addQuery(SearchQuery({"search_3"}), {site});
		REQUIRE(history.entries().count() == 3);

		history.removeQuery(SearchQuery({"search_2"}), {site});
		REQUIRE(history.entries().count() == 2);

		REQUIRE(history.entries()[0]->query.tags == QStringList("search_1"));
		REQUIRE(history.entries()[1]->query.tags == QStringList("search_3"));
	}
}
