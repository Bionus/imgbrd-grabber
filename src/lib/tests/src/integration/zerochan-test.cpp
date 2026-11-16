#include <algorithm>
#include <QFile>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QStringList>
#include "models/api/api.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/search-query/search-query.h"
#include "models/site.h"
#include "models/source.h"
#include "tags/tag.h"
#include "catch.h"
#include "integration-helpers.h"
#include "source-helpers.h"


TEST_CASE("Zerochan")
{
	SECTION("Html")
	{
		setupSource("Zerochan");
		setupSite("Zerochan", "www.zerochan.net");
		const QScopedPointer<Profile> profile(makeProfile());
		Source *source = profile->getSources().value("Zerochan");
		REQUIRE(source != nullptr);
		Site site("www.zerochan.net", source, profile.data());
		Api *api = source->getApi("Html");
		REQUIRE(api != nullptr);
		Page page(profile.data(), &site, { &site }, SearchQuery(QStringList { "Hatsune_Miku" }), 1, 48, {}, false);
		const auto fixture = [](const QString &name) {
			QFile file("tests/resources/pages/www.zerochan.net/" + name);
			REQUIRE(file.open(QFile::ReadOnly));
			return QString::fromUtf8(file.readAll());
		};

		SECTION("unfiltered listing")
		{
			const auto parsed = api->parsePage(&page, fixture("results.html"), 200, 0);
			REQUIRE(parsed.error.isEmpty());
			REQUIRE(api->forcedLimit() == 48);
			REQUIRE(parsed.images.count() == 48);
			const auto &image = parsed.images.first();
			REQUIRE(image->id() == 4736754);
			REQUIRE(image->fileUrl().toString() == "https://static.zerochan.net/Hiwatari.Kanae.full.4736754.png");
			REQUIRE(image->width() == 1881);
			REQUIRE(image->height() == 2165);
			REQUIRE(image->fileSize() > 900000);
			REQUIRE(image->token<QString>("author") == "Yoga Widya Prasetyo");
			REQUIRE(parsed.imageCount == 4164031);
			REQUIRE(parsed.pageCount == 86751);
			REQUIRE(parsed.urlNextPage.toString() == "?s=id&p=2");

			const auto child = std::find_if(parsed.images.cbegin(), parsed.images.cend(), [](const auto &entry) {
				return entry->id() == 4734342;
			});
			REQUIRE(child != parsed.images.cend());
			REQUIRE((*child)->width() == 3840);
			REQUIRE((*child)->height() == 2160);
			REQUIRE((*child)->fileSize() == 6014 * 1024);
			REQUIRE((*child)->token<QString>("author") == "Roberto Manfinfla1384");
			REQUIRE((*child)->fileUrl().toString() == "https://static.zerochan.net/Sannin.full.4734342.jpg");
		}

		SECTION("search")
		{
			const auto first = api->parsePage(&page, fixture("search.html"), 200, 0);
			REQUIRE(first.error.isEmpty());
			REQUIRE(first.images.count() == 48);
			REQUIRE(first.images.first()->id() == 4734826);
			REQUIRE(first.imageCount == 59951);
			REQUIRE(first.pageCount == 1754);
			REQUIRE(first.urlNextPage.toString() == "?s=id&p=2");
		}

		SECTION("structured metadata takes precedence")
		{
			QString html = fixture("results.html");
			html.replace("1881&#10005;2165 937kb png", "1881&#10005;2165 937kb jpg");
			const auto parsed = api->parsePage(&page, html, 200, 0);
			REQUIRE(parsed.error.isEmpty());
			REQUIRE(parsed.images.count() == 48);
			REQUIRE(parsed.images.first()->fileUrl().toString() == "https://static.zerochan.net/Hiwatari.Kanae.full.4736754.png");
		}

		SECTION("details")
		{
			const auto details = api->parseDetails(fixture("details.html"), 200, &site);
			REQUIRE(details.error.isEmpty());
			REQUIRE(details.imageUrl == "https://static.zerochan.net/Hiwatari.Kanae.full.4736754.png");
			REQUIRE(details.createdAt.toUTC().toString(Qt::ISODate) == "2026-11-16T00:16:21Z");
			REQUIRE(details.tags.count() == 6);
			QMap<QString, QString> types;
			for (const auto &tag : details.tags) {
				types.insert(tag.text(), tag.type().name());
			}
			REQUIRE(types.value("Pixiv_Id_104267145") == "artist");
			REQUIRE(types.value("Gakuen_iDOLM@STER") == "copyright");
			REQUIRE(types.value("Hiwatari_Kanae") == "character");
		}
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
