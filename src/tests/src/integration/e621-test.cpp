#include <QSharedPointer>
#include <QStringList>
#include "models/image.h"
#include "tags/tag.h"
#include "catch.h"
#include "integration-helpers.h"


TEST_CASE("E621")
{
	SECTION("HTML SWF search")
	{
		QList<QSharedPointer<Image>> images = getImages("E621", "e621.net", "regex", "swf rating:safe", "results.html");

		// Convert results
		QStringList md5s, urls;
		md5s.reserve(images.count());
		urls.reserve(images.count());
		for (const auto &img : images) {
			md5s.append(img->md5());
			urls.append(img->url().toString());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "7da0468faa919931761286a7e7ae63b6" << "02ceb824579f74ac005d856992fb8da8" << "b224993c9c150fc7010c5c4d96ac4b10";
		REQUIRE(images.count() == 20);
		REQUIRE(md5s == expected);

		// Compare URLs
		QStringList expectedUrls = QStringList()
			<< "https://static1.e621.net/data/7d/a0/7da0468faa919931761286a7e7ae63b6.swf"
			<< "https://static1.e621.net/data/02/ce/02ceb824579f74ac005d856992fb8da8.swf"
			<< "https://static1.e621.net/data/b2/24/b224993c9c150fc7010c5c4d96ac4b10.swf";
		QStringList actualUrls = urls.mid(0, 3);
		REQUIRE(actualUrls == expectedUrls);
	}

	SECTION("JSON search with typed tags")
	{
		QList<QSharedPointer<Image>> images = getImages("E621", "e621.net", "json", "rating:safe", "results.json");
		REQUIRE(!images.isEmpty());

		QList<Tag> tags = images.first()->tags();
		REQUIRE(tags.count() == 24);

		REQUIRE(tags[16].text() == QString("zenfry"));
		REQUIRE(tags[16].type().name() == QString("artist"));
	}

	SECTION("HTML tags")
	{
		QList<Tag> tags = getTags("E621", "e621.net", "regex", "tags.html");

		REQUIRE(tags.count() == 75);

		REQUIRE(tags[1].text() == QString("mammal"));
		REQUIRE(tags[1].count() == 1770103);
		REQUIRE(tags[1].type().name() == QString("species"));
	}

	SECTION("JSON tags")
	{
		QList<Tag> tags = getTags("E621", "e621.net", "json", "tags.json");

		REQUIRE(tags.count() == 75);

		REQUIRE(tags[1].text() == QString("mammal"));
		REQUIRE(tags[1].count() == 1770104);
		REQUIRE(tags[1].type().name() == QString("species"));
	}
}
