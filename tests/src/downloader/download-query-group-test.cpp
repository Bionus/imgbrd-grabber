#include "downloader/download-query-group.h"
#include "models/profile.h"
#include "models/site.h"
#include "catch.h"
#include "source-helpers.h"


TEST_CASE("DownloadQueryGroup")
{
	SECTION("Compare")
	{
		DownloadQueryGroup a(QStringList() << "tags", 1, 2, 3, QStringList() << "postFiltering", true, nullptr, "filename", "path");
		DownloadQueryGroup b(QStringList() << "tags", 1, 2, 3, QStringList() << "postFiltering", true, nullptr, "filename", "path");
		DownloadQueryGroup c(QStringList() << "tags", 1, 3, 3, QStringList() << "postFiltering", true, nullptr, "filename", "path");
		DownloadQueryGroup d(QStringList() << "tags", 1, 3, 3, QStringList() << "postFiltering", true, nullptr, "filename", "path");

		d.progressVal = 37;
		d.progressFinished = false;

		REQUIRE(a == b);
		REQUIRE(b == a);
		REQUIRE(a != c);
		REQUIRE(b != c);
		REQUIRE(c == c);
		REQUIRE(c == d); // The progress status must NOT be checked
	}

	SECTION("Serialization")
	{
		setupSource("Danbooru (2.0)");
		setupSite("Danbooru (2.0)", "danbooru.donmai.us");

		Profile profile("tests/resources/");
		Site *site = profile.getSites().value("danbooru.donmai.us");

		DownloadQueryGroup original(QStringList() << "tags", 1, 2, 3, QStringList() << "postFiltering", true, site, "filename", "path");
		original.progressVal = 37;
		original.progressFinished = false;

		QJsonObject json;
		original.write(json);

		DownloadQueryGroup dest;
		dest.read(json, &profile);

		REQUIRE(dest.query.tags == QStringList() << "tags");
		REQUIRE(dest.page == 1);
		REQUIRE(dest.perpage == 2);
		REQUIRE(dest.total == 3);
		REQUIRE(dest.postFiltering == QStringList() << "postFiltering");
		REQUIRE(dest.getBlacklisted);
		REQUIRE(dest.site == site);
		REQUIRE(dest.filename == QString("filename"));
		REQUIRE(dest.path == QString("path"));
		REQUIRE(dest.progressVal == 37);
		REQUIRE(!dest.progressFinished);
	}
}
