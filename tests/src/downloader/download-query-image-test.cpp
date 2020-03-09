#include <QMap>
#include <QSharedPointer>
#include "downloader/download-query-image.h"
#include "models/image.h"
#include "models/image-factory.h"
#include "models/profile.h"
#include "models/site.h"
#include "catch.h"


TEST_CASE("DownloadQueryImage")
{
	Profile profile("tests/resources/");
	Site *site = profile.getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	SECTION("Compare")
	{
		auto img1 = QSharedPointer<Image>(new Image(site, {{ "id", "1" }}, &profile));
		auto img2 = QSharedPointer<Image>(new Image(site, {{ "id", "2" }}, &profile));

		DownloadQueryImage a(img1, site, "filename", "path");
		DownloadQueryImage b(img1, site, "filename", "path");
		DownloadQueryImage c(img2, site, "filename", "path");

		REQUIRE(a == b);
		REQUIRE(b == a);
		REQUIRE(a != c);
		REQUIRE(b != c);
		REQUIRE(c == c);
	}

	SECTION("Serialization")
	{
		QMap<QString, QString> details = {
			{ "id", "1" },
			{ "md5", "md5" },
			{ "rating", "rating" },
			{ "tags", "tags" },
			{ "file_url", "https://test.com/fileUrl" },
			{ "date", "2016-08-26T16:26:30+01:00" },
			{ "search", "search" },
		};
		auto img = ImageFactory::build(site, details, &profile);
		DownloadQueryImage original(img, site, "filename", "path");

		QJsonObject json;
		original.write(json);

		DownloadQueryImage dest;
		dest.read(json, &profile);

		REQUIRE(static_cast<int>(dest.image->id()) == 1);
		REQUIRE(dest.image->md5() == QString("md5"));
		REQUIRE(dest.image->token<QString>("rating") == QString("rating"));
		REQUIRE(dest.image->tagsString() == QStringList() << "tags");
		REQUIRE(dest.image->fileUrl().toString() == QString("https://test.com/fileUrl"));
		REQUIRE(dest.image->createdAt().toString("yyyy-MM-dd HH:mm:ss") == QString("2016-08-26 16:26:30"));
		REQUIRE(dest.image->search() == QStringList() << "search");

		REQUIRE(dest.site == site);
		REQUIRE(dest.filename == QString("filename"));
		REQUIRE(dest.path == QString("path"));
	}
}
