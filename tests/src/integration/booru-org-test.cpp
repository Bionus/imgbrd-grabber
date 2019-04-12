#include <QStringList>
#include "models/image.h"
#include "tags/tag.h"
#include "catch.h"
#include "integration-helpers.h"


TEST_CASE("Booru.org")
{
	SECTION("Html")
	{
		QList<Image*> images = getImages("Gelbooru (0.1)", "rm.booru.org", "regex", "rating:safe", "results.html");

		// Convert results
		QStringList md5s;
		md5s.reserve(images.count());
		for (Image *img : images) {
			md5s.append(img->md5());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "88407041cfd2d8358dda2f8699bfe98d84a7cf74" << "e0c2ddaf9403901cc1e293bcd369806d1deffd95" << "44f0f9560431d1b61ba1e9c401fdb3cc75920b38";
		REQUIRE(images.count() == 20);
		REQUIRE(md5s == expected);
	}

	SECTION("PageTags")
	{
		QList<Tag> tags = getPageTags("Gelbooru (0.1)", "rm.booru.org", "regex", "rating:safe", "results.html");

		REQUIRE(tags.count() == 5);

		REQUIRE(tags[0].text() == QString("barasuishou"));
		REQUIRE(tags[0].count() == 4825);
		REQUIRE(tags[1].text() == QString("image"));
		REQUIRE(tags[1].count() == 94810);
		REQUIRE(tags[2].text() == QString("rozen_maiden"));
		REQUIRE(tags[2].count() == 125996);
	}
}
