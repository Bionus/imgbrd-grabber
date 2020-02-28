#include <QSharedPointer>
#include <QStringList>
#include "models/image.h"
#include "tags/tag.h"
#include "catch.h"
#include "integration-helpers.h"


TEST_CASE("Danbooru")
{
	SECTION("Html")
	{
		QList<QSharedPointer<Image>> images = getImages("Danbooru (2.0)", "danbooru.donmai.us", "regex", "rating:safe", "results.html");

		// Convert results
		QStringList md5s;
		md5s.reserve(images.count());
		for (const auto &img : images) {
			md5s.append(img->md5());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "12a54c9a24868a6c717759f1dfef5864" << "b46086a869da3443181f7798c6918058" << "9aeff7f9ffddb7c6db36133be4ad4ca3";
		REQUIRE(images.count() == 20);
		REQUIRE(md5s == expected);
	}

	SECTION("Xml")
	{
		QList<QSharedPointer<Image>> images = getImages("Danbooru (2.0)", "danbooru.donmai.us", "xml", "rating:safe", "results.xml");

		// Convert results
		QStringList md5s;
		md5s.reserve(images.count());
		for (const auto &img : images) {
			md5s.append(img->md5());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "test98bf686ca7910ee0ad48e59ba99807d3" << "e5d074c5fe05e1493372e7224d2d198f" << "1e774a4a9d080611fde61e58625e038e";
		REQUIRE(images.count() == 20);
		REQUIRE(md5s == expected);
	}

	SECTION("PageTags")
	{
		QList<Tag> tags = getPageTags("Danbooru (2.0)", "danbooru.donmai.us", "regex", "rating:safe", "results.html");

		REQUIRE(tags.count() == 25);

		REQUIRE(tags[0].text() == QString("solo"));
		REQUIRE(tags[0].count() == 1805000);
		REQUIRE(tags[1].text() == QString("looking_at_viewer"));
		REQUIRE(tags[1].count() == 707000);
		REQUIRE(tags[2].text() == QString("1girl"));
		REQUIRE(tags[2].count() == 2177000);
	}

	SECTION("HtmlTags")
	{
		QList<Tag> tags = getTags("Danbooru (2.0)", "danbooru.donmai.us", "regex", "tags.html");

		REQUIRE(tags.count() == 100);

		REQUIRE(tags[1].text() == QString("apollo_star"));
		REQUIRE(tags[1].count() == 1);
		REQUIRE(tags[1].type().name() == QString("artist"));
	}

	SECTION("XmlTags")
	{
		QList<Tag> tags = getTags("Danbooru (2.0)", "danbooru.donmai.us", "xml", "tags.xml");

		REQUIRE(tags.count() == 100);

		REQUIRE(tags[1].text() == QString("walkr"));
		REQUIRE(tags[1].count() == 1);
		REQUIRE(tags[1].type().name() == QString("copyright"));
	}

	SECTION("JsonTags")
	{
		QList<Tag> tags = getTags("Danbooru (2.0)", "danbooru.donmai.us", "json", "tags.json");

		REQUIRE(tags.count() == 100);

		REQUIRE(tags[1].text() == QString("walkr"));
		REQUIRE(tags[1].count() == 1);
		REQUIRE(tags[1].type().name() == QString("copyright"));
	}
}
