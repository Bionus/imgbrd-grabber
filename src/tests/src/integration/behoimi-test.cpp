#include <QSharedPointer>
#include <QStringList>
#include "models/image.h"
#include "tags/tag.h"
#include "catch.h"
#include "integration-helpers.h"


TEST_CASE("Behoimi")
{
	SECTION("Html")
	{
		QList<QSharedPointer<Image>> images = getImages("Danbooru", "behoimi.org", "regex", "blue_legwear rating:safe", "results.html");

		// Convert results
		QStringList md5s;
		md5s.reserve(images.count());
		for (const auto &img : images) {
			md5s.append(img->md5());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "2bdf03f0d3e7c5dcdfadaedc0434093e" << "1073770a3b0b565e1d0593620f28c0d6" << "5a49bcb7e90322c1edf866900e61ba1f";
		REQUIRE(images.count() == 20);
		REQUIRE(md5s == expected);
	}

	SECTION("Xml")
	{
		QList<QSharedPointer<Image>> images = getImages("Danbooru", "behoimi.org", "xml", "rating:safe", "results.xml");

		// Convert results
		QStringList md5s;
		md5s.reserve(images.count());
		for (const auto &img : images) {
			md5s.append(img->md5());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "129577287dc57940398169481e7423cb" << "5a4d01cfbecc2a293d46df70144d6441" << "af7b48d271422dfdeb24c6dd102a8d50";
		REQUIRE(images.count() == 20);
		REQUIRE(md5s == expected);
	}

	SECTION("Json")
	{
		QList<QSharedPointer<Image>> images = getImages("Danbooru", "behoimi.org", "json", "rating:safe", "results.json");

		// Convert results
		QStringList md5s;
		md5s.reserve(images.count());
		for (const auto &img : images) {
			md5s.append(img->md5());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "129577287dc57940398169481e7423cb" << "5a4d01cfbecc2a293d46df70144d6441" << "af7b48d271422dfdeb24c6dd102a8d50";
		REQUIRE(images.count() == 20);
		REQUIRE(md5s == expected);
	}

	SECTION("PageTags")
	{
		QList<Tag> tags = getPageTags("Danbooru", "behoimi.org", "regex", "blue_legwear rating:safe", "results.html");

		REQUIRE(tags.count() == 25);

		REQUIRE(tags[0].text() == QString("blue_legwear"));
		REQUIRE(tags[0].count() == 295);
		REQUIRE(tags[1].text() == QString("cosplay"));
		REQUIRE(tags[1].count() == 295);
		REQUIRE(tags[2].text() == QString("thighhighs"));
		REQUIRE(tags[2].count() == 222);
	}

	SECTION("HtmlTags")
	{
		QList<Tag> tags = getTags("Danbooru", "behoimi.org", "regex", "tags.html");

		REQUIRE(tags.count() == 50);

		REQUIRE(tags[2].text() == QString("104"));
		REQUIRE(tags[2].count() == 9);
		REQUIRE(tags[2].type().name() == QString("model"));
	}

	SECTION("XmlTags")
	{
		QList<Tag> tags = getTags("Danbooru", "behoimi.org", "xml", "tags.xml");

		REQUIRE(tags.count() == 100);

		REQUIRE(tags[3].text() == QString("okubo_mariko"));
		REQUIRE(tags[3].count() == 286);
		REQUIRE(tags[3].type().name() == QString("model"));
	}

	SECTION("JsonTags")
	{
		QList<Tag> tags = getTags("Danbooru", "behoimi.org", "json", "tags.json");

		REQUIRE(tags.count() == 100);

		REQUIRE(tags[1].text() == QString("07_ghost"));
		REQUIRE(tags[1].count() == 3);
		REQUIRE(tags[1].type().name() == QString("copyright"));
	}
}
