#include <QSharedPointer>
#include <QStringList>
#include "models/image.h"
#include "tags/tag.h"
#include "catch.h"
#include "integration-helpers.h"


TEST_CASE("Gelbooru")
{
	SECTION("Html")
	{
		QList<QSharedPointer<Image>> images = getImages("Gelbooru (0.2)", "gelbooru.com", "regex", "rating:safe", "results.html");

		// Convert results
		QStringList md5s;
		md5s.reserve(images.count());
		for (const auto &img : images) {
			md5s.append(img->md5());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "2fd892ef1143793644e0b8f38e1c8849" << "b29ccbb2fcbeddad7a95b93c822ecbc0" << "31bf5040bafd4ebda1ae241857476b65";
		REQUIRE(images.count() == 20);
		REQUIRE(md5s == expected);
	}

	SECTION("Xml")
	{
		QList<QSharedPointer<Image>> images = getImages("Gelbooru (0.2)", "gelbooru.com", "xml", "rating:safe", "results.xml");

		// Convert results
		QStringList md5s;
		md5s.reserve(images.count());
		for (const auto &img : images) {
			md5s.append(img->md5());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "e70f631ce2bc1ab02b371489d81dceaa" << "ea2d6cd3dbe115401fc23b4ae3e7e7ab" << "67854632115557de382f26669228cec6";
		REQUIRE(images.count() == 20);
		REQUIRE(md5s == expected);
	}

	SECTION("PageTags")
	{
		QList<Tag> tags = getPageTags("Gelbooru (0.2)", "gelbooru.com", "regex", "rating:safe", "results.html");

		REQUIRE(tags.count() == 53);

		REQUIRE(tags[0].text() == QString("00s"));
		REQUIRE(tags[0].count() == 255610);
		REQUIRE(tags[1].text() == QString("1girl"));
		REQUIRE(tags[1].count() == 2302988);
		REQUIRE(tags[2].text() == QString("aqua_hair"));
		REQUIRE(tags[2].count() == 60142);
	}

	SECTION("HtmlTags")
	{
		QList<Tag> tags = getTags("Gelbooru (0.2)", "gelbooru.com", "regex", "tags.html");

		REQUIRE(tags.count() == 50);

		REQUIRE(tags[3].text() == QString("nami_(one_piece)"));
		REQUIRE(tags[3].count() == 5594);
		REQUIRE(tags[3].type().name() == QString("character"));
	}
}
