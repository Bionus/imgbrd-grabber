#include "catch.h"
#include <QList>
#include <QTextStream>
#include "models/image.h"
#include "models/profile.h"
#include "printers/json-printer.h"
#include "source-helpers.h"
#include "tags/tag.h"


TEST_CASE("JsonPrinter")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");
	const QScopedPointer<Profile> profile(makeProfile());
	Site *site = profile->getSites().value("danbooru.donmai.us");

	QString output;
	QTextStream stream(&output);
	JsonPrinter printer(&stream, profile.data());

	SECTION("Print int")
	{
		SECTION("Zero")
		{
			printer.print(0);
			REQUIRE(output == "0\n");
		}

		SECTION("Positive")
		{
			printer.print(123);
			REQUIRE(output == "123\n");
		}

		SECTION("Negative")
		{
			printer.print(-123);
			REQUIRE(output == "-123\n");
		}
	}

	SECTION("Print string")
	{
		SECTION("Empty string")
		{
			printer.print("");
			REQUIRE(output == "\n");
		}

		SECTION("Simple string")
		{
			printer.print("test");
			REQUIRE(output == "test\n");
		}
	}

	SECTION("Print image")
	{
		Image image(site, {{ "id", "123" }, { "md5", "test_md5" }, { "file_url", "https://test.com/image.jpg" }}, profile.data());

		printer.print(image);
		REQUIRE(output.contains(R"("id": "123",)"));
		REQUIRE(output.contains(R"("md5": "test_md5",)"));
		REQUIRE(output.contains(R"("url_file": "https://test.com/image.jpg",)"));
	}

	SECTION("Print images")
	{
		QList<QSharedPointer<Image>> images {
			QSharedPointer<Image>(new Image(site, {{ "id", "123" }, { "md5", "test_md5_1" }, { "file_url", "https://test.com/image_1.jpg" }}, profile.data())),
			QSharedPointer<Image>(new Image(site, {{ "id", "456" }, { "md5", "test_md5_2" }, { "file_url", "https://test.com/image_2.jpg" }}, profile.data())),
			QSharedPointer<Image>(new Image(site, {{ "id", "789" }, { "md5", "test_md5_3" }, { "file_url", "https://test.com/image_3.jpg" }}, profile.data())),
		};

		printer.print(images);
		REQUIRE(output.contains(R"("id": "123",)"));
		REQUIRE(output.contains(R"("id": "456",)"));
		REQUIRE(output.contains(R"("id": "789",)"));
	}

	SECTION("Print tag")
	{
		Tag tag("test_tag", "artist", 123);

		printer.print(tag, site);
		REQUIRE(output == R"({
    "count": 123,
    "text": "test_tag",
    "type": "artist"
}
)");
	}

	SECTION("Print tags")
	{
		QList<Tag> tags {
			Tag("test_tag_1", "artist", 123),
			Tag("test_tag_2", "general", 45),
			Tag("test_tag_3", "copyright", 678),
		};

		printer.print(tags, site);
		REQUIRE(output == R"([
    {
        "count": 123,
        "text": "test_tag_1",
        "type": "artist"
    },
    {
        "count": 45,
        "text": "test_tag_2",
        "type": "general"
    },
    {
        "count": 678,
        "text": "test_tag_3",
        "type": "copyright"
    }
]
)");
	}
}
