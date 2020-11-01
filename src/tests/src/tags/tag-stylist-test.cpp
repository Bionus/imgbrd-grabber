#include <QSettings>
#include "models/favorite.h"
#include "models/profile.h"
#include "tags/tag.h"
#include "tags/tag-stylist.h"
#include "catch.h"

using Catch::Matchers::Matches;


// Ignore those tests on MacOS because of font difference
// TODO(Bionus): re-enable them and use matching or something to be more resistent
#if !defined(Q_OS_MACOS)

void assertSort(QSettings *settings, const QString &sort, const QStringList &expectedOrder)
{
	settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	settings->setValue("Coloring/Colors/artists", "#aa0000");
	settings->setValue("Coloring/Fonts/generals", ",8.25,-1,5,50,0,0,0,0,0");
	settings->setValue("Coloring/Colors/generals", "#aa0000");
	settings->setValue("Coloring/Fonts/copyrights", ",8.25,-1,5,50,0,0,0,0,0");
	settings->setValue("Coloring/Colors/copyrights", "#aa0000");

	auto tags = QList<Tag>
	{
		Tag("tag3", "general", 3, QStringList()),
		Tag("tag2", "copyright", 2, QStringList()),
		Tag("tag1", "artist", 1, QStringList()),
	};

	TagStylist stylist(new Profile(settings, QList<Favorite>()));
	QStringList actual = stylist.stylished(tags, false, false, sort);

	REQUIRE(actual.count() == expectedOrder.count());

	QString format = "<a href=\"%1\" style=\"color:#aa0000; font-family:'[^']*'; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">%1</a>";
	QStringList expected;
	for (int i = 0; i < expectedOrder.count(); ++i) {
		const QString &tag = expectedOrder[i];
		REQUIRE_THAT(actual[i].toStdString(), Matches(format.arg(tag).toStdString()));
	}

}


TEST_CASE("TagStylist")
{
	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
	
	SECTION("Basic")
	{
		settings.setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
		settings.setValue("Coloring/Colors/artists", "#aa0000");

		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		QList<Favorite> favorites;
		favorites.append(Favorite("tag_other", 50, QDateTime::currentDateTime()));

		TagStylist stylist(new Profile(&settings, favorites));
		QString actual = stylist.stylished(QList<Tag>() << tag).join("");
		std::string expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:'[^']*'; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
		REQUIRE_THAT(actual.toStdString(), Matches(expected));
	}

	SECTION("Ignored")
	{
		settings.setValue("Coloring/Fonts/ignoreds", ",8.25,-1,5,50,0,0,0,0,0");
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		Profile pro(&settings, QList<Favorite>());
		pro.addIgnored("tag_text");

		TagStylist stylist(&pro);
		QString actual = stylist.stylished(QList<Tag>() << tag).join("");
		std::string expected = "<a href=\"tag_text\" style=\"color:#999999; font-family:'[^']*'; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
		REQUIRE_THAT(actual.toStdString(), Matches(expected));
	}

	SECTION("Blacklisted")
	{
		settings.setValue("Coloring/Fonts/blacklisteds", ",8.25,-1,5,50,0,0,0,0,0");
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		Profile pro(&settings, QList<Favorite>());
		pro.addBlacklistedTag("tag_text");

		TagStylist stylist(&pro);
		QString actual = stylist.stylished(QList<Tag>() << tag).join("");
		std::string expected = "<a href=\"tag_text\" style=\"color:#000000; font-family:'[^']*'; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
		REQUIRE_THAT(actual.toStdString(), Matches(expected));
	}

	SECTION("Favorite")
	{
		settings.setValue("Coloring/Fonts/favorites", ",8.25,-1,5,50,0,0,0,0,0");
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		QList<Favorite> favorites;
		favorites.append(Favorite("tag_text", 50, QDateTime::currentDateTime()));

		TagStylist stylist(new Profile(&settings, favorites));
		QString actual = stylist.stylished(QList<Tag>() << tag).join("");
		std::string expected = "<a href=\"tag_text\" style=\"color:#ffc0cb; font-family:'[^']*'; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
		REQUIRE_THAT(actual.toStdString(), Matches(expected));
	}

	SECTION("KeptForLater")
	{
		settings.setValue("Coloring/Fonts/keptForLater", ",8.25,-1,5,50,0,0,0,0,0");
		settings.setValue("Coloring/Colors/keptForLater", "#aa0000");

		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		QStringList keptForLater = QStringList() << "tag_text";

		TagStylist stylist(new Profile(&settings, QList<Favorite>(), keptForLater));
		QString actual = stylist.stylished(QList<Tag>() << tag).join("");
		std::string expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:'[^']*'; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
		REQUIRE_THAT(actual.toStdString(), Matches(expected));
	}

	SECTION("WithCount")
	{
		settings.setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
		settings.setValue("Coloring/Colors/artists", "#aa0000");

		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		TagStylist stylist(new Profile(&settings, QList<Favorite>()));
		QString actual = stylist.stylished(QList<Tag>() << tag, true).join("");
		std::string expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:'[^']*'; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a> <span style=\"color:#aaa\">\\(123\\)</span>";
		REQUIRE_THAT(actual.toStdString(), Matches(expected));
	}

	SECTION("Sort")
	{
		SECTION("Name")
		{
			assertSort(&settings, "name", QStringList() << "tag1" << "tag2" << "tag3");
		}

		SECTION("Type")
		{
			assertSort(&settings, "type", QStringList() << "tag2" << "tag1" << "tag3");
		}

		SECTION("Count")
		{
			assertSort(&settings, "count", QStringList() << "tag3" << "tag2" << "tag1");
		}
	}
}

#endif
