#include <QSharedPointer>
#include <QStringList>
#include "models/image.h"
#include "tags/tag.h"
#include "catch.h"
#include "integration-helpers.h"


TEST_CASE("Zerochan")
{
	SECTION("Html")
	{
		QList<QSharedPointer<Image>> images = getImages("Zerochan", "www.zerochan.net", "regex", "Touhou", "results.html");

		// Convert results
		QList<qulonglong> ids;
		ids.reserve(images.count());
		for (const auto &img : images) {
			ids.append(img->id());
		}

		// Check results
		ids = ids.mid(0, 3);
		QList<qulonglong> expected = QList<qulonglong>() << 2034435 << 2034432 << 2034431;
		REQUIRE(images.count() == 20);
		REQUIRE(ids == expected);
	}

	SECTION("Rss")
	{
		QList<QSharedPointer<Image>> images = getImages("Zerochan", "www.zerochan.net", "rss", "Touhou", "results.rss");

		// Convert results
		QList<qulonglong> ids;
		ids.reserve(images.count());
		for (const auto &img : images) {
			ids.append(img->id());
		}

		// Check results
		ids = ids.mid(0, 3);
		QList<qulonglong> expected = QList<qulonglong>() << 2034435 << 2034432 << 2034431;
		REQUIRE(images.count() == 20);
		REQUIRE(ids == expected);
	}
}
