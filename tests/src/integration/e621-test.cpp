#include <QStringList>
#include "models/image.h"
#include "tags/tag.h"
#include "catch.h"
#include "integration-helpers.h"


TEST_CASE("E621")
{
	SECTION("SwfUrls")
	{
		QList<Image*> images = getImages("Danbooru", "e621.net", "regex", "swf rating:safe", "results.html");

		// Convert results
		QStringList md5s, urls;
		md5s.reserve(images.count());
		urls.reserve(images.count());
		for (Image *img : images) {
			md5s.append(img->md5());
			urls.append(img->url().toString());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "cb0523430ab3a75216fe1b3a3a42cac5" << "4533e0a1bf7b132038f7ab3864ecd027" << "d8461800f2a107f2d928fcbca00f6019";
		REQUIRE(images.count() == 20);
		REQUIRE(md5s == expected);

		// Compare URLs
		QStringList expectedUrls = QStringList()
			<< "https://static1.e621.net/data/cb/05/cb0523430ab3a75216fe1b3a3a42cac5.swf"
			<< "https://static1.e621.net/data/45/33/4533e0a1bf7b132038f7ab3864ecd027.swf"
			<< "https://static1.e621.net/data/d8/46/d8461800f2a107f2d928fcbca00f6019.swf";
		QStringList actualUrls = urls.mid(0, 3);
		REQUIRE(actualUrls == expectedUrls);
	}

	SECTION("XmlTypedTags")
	{
		QList<Image*> images = getImages("Danbooru", "e621.net", "xml", "rating:safe", "results-typed.xml");
		REQUIRE(!images.isEmpty());

		QList<Tag> tags = images.first()->tags();
		REQUIRE(tags.count() == 22);

		REQUIRE(tags[0].text() == QString("female"));
		REQUIRE(tags[0].type().name() == QString("general"));
		REQUIRE(tags[21].text() == QString("mammal"));
		REQUIRE(tags[21].type().name() == QString("species"));
	}

	SECTION("JsonTypedTags")
	{
		QList<Image*> images = getImages("Danbooru", "e621.net", "json", "rating:safe", "results-typed.json");
		REQUIRE(!images.isEmpty());

		QList<Tag> tags = images.first()->tags();
		REQUIRE(tags.count() == 22);

		REQUIRE(tags[21].text() == QString("equine"));
		REQUIRE(tags[21].type().name() == QString("species"));
	}

	SECTION("HtmlTags")
	{
		QList<Tag> tags = getTags("Danbooru", "e621.net", "regex", "tags.html");

		REQUIRE(tags.count() == 100);

		REQUIRE(tags[0].text() == QString("mammal"));
		REQUIRE(tags[0].count() == 907884);
		REQUIRE(tags[0].type().name() == QString("species"));
	}

	SECTION("XmlTags")
	{
		QList<Tag> tags = getTags("Danbooru", "e621.net", "xml", "tags.xml");

		REQUIRE(tags.count() == 100);

		REQUIRE(tags[0].text() == QString("mammal"));
		REQUIRE(tags[0].count() == 866534);
		REQUIRE(tags[0].type().name() == QString("species"));
	}

	SECTION("JsonTags")
	{
		QList<Tag> tags = getTags("Danbooru", "e621.net", "json", "tags.json");

		REQUIRE(tags.count() == 100);

		REQUIRE(tags[0].text() == QString("mammal"));
		REQUIRE(tags[0].count() == 866534);
		REQUIRE(tags[0].type().name() == QString("species"));
	}
}
