#include <QSharedPointer>
#include <QStringList>
#include "models/image.h"
#include "tags/tag.h"
#include "catch.h"
#include "integration-helpers.h"


TEST_CASE("Derpibooru")
{
	SECTION("Html")
	{
		QList<QSharedPointer<Image>> images = getImages("Philomena", "derpibooru.org", "regex", "safe", "results.html");

		// Convert results
		QList<qulonglong> ids;
		ids.reserve(images.count());
		for (const auto &img : images) {
			ids.append(img->id());
		}

		// Check results
		ids = ids.mid(0, 3);
		QList<qulonglong> expected = QList<qulonglong>() << 1752855 << 1752854 << 1752853;
		REQUIRE(images.count() == 15);
		REQUIRE(ids == expected);
	}

	SECTION("Json")
	{
		QList<QSharedPointer<Image>> images = getImages("Philomena", "derpibooru.org", "json", "safe", "results.json");

		// Convert results
		QList<qulonglong> ids;
		ids.reserve(images.count());
		for (const auto &img : images) {
			ids.append(img->id());
		}

		// Check results
		ids = ids.mid(0, 3);
		QList<qulonglong> expected = QList<qulonglong>() << 2316404 << 2316401 << 2316400;
		REQUIRE(images.count() == 5);
		REQUIRE(ids == expected);
	}

	SECTION("HtmlTags")
	{
		QList<Tag> tags = getTags("Philomena", "derpibooru.org", "regex", "tags.html");

		REQUIRE(tags.count() == 250);

		REQUIRE(tags[1].text() == QString("solo"));
		REQUIRE(tags[1].count() == 599506);
		REQUIRE(tags[1].type().isUnknown() == true);
	}

	SECTION("JsonTags")
	{
		QList<Tag> tags = getTags("Philomena", "derpibooru.org", "json", "tags.json");

		REQUIRE(tags.count() == 10);

		REQUIRE(tags[1].id() == 42350);
		REQUIRE(tags[1].text() == QString("solo"));
		REQUIRE(tags[1].count() == 938816);
		REQUIRE(tags[1].type().isUnknown() == true);
	}
}
