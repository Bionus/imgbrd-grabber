#include <QDebug>
#include "tags/tag.h"
#include "tags/tag-database-sqlite.h"
#include "catch.h"


TEST_CASE("TagDatabaseSqliteTest")
{
	TagDatabaseSqlite database("tests/resources/tag-types.txt", "tests/resources/tags.db");

	database.open();
	database.load();
	database.setTags(QList<Tag>());

	SECTION("Empty contains none")
	{
		database.setTags(QList<Tag>());

		QTime timer;
		timer.start();
		QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");
		int elapsed = timer.elapsed();

		REQUIRE(types.count() == 0);
		qDebug() << "Elapsed" << elapsed << "ms";
		REQUIRE(elapsed < 20);

		REQUIRE(database.count() == 0);
	}

	SECTION("Filled contains all")
	{
		database.setTags(QList<Tag>() << Tag("tag1", TagType("general")) << Tag("tag2", TagType("artist")) << Tag("tag3", TagType("copyright")) << Tag("tag4", TagType("character")));

		QTime timer;
		timer.start();
		QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");
		int elapsed = timer.elapsed();

		REQUIRE(types.count() == 2);
		REQUIRE(types.contains("tag1") == true);
		REQUIRE(types.contains("tag3") == true);
		REQUIRE(types.value("tag1").name() == QString("general"));
		REQUIRE(types.value("tag3").name() == QString("copyright"));
		qDebug() << "Elapsed" << elapsed << "ms";
		REQUIRE(elapsed < 20);

		REQUIRE(database.count() == 4);
	}

	SECTION("Filled contains some")
	{
		database.setTags(QList<Tag>() << Tag("tag1", TagType("general")) << Tag("tag2", TagType("artist")) << Tag("tag3", TagType("copyright")) << Tag("tag4", TagType("character")));

		QTime timer;
		timer.start();
		QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3" << "tag5" << "missing_tag");
		int elapsed = timer.elapsed();

		REQUIRE(types.count() == 2);
		REQUIRE(types.contains("tag1") == true);
		REQUIRE(types.contains("tag3") == true);
		REQUIRE(types.contains("tag5") == false);
		REQUIRE(types.contains("missing_tag") == false);
		REQUIRE(types.value("tag1").name() == QString("general"));
		REQUIRE(types.value("tag3").name() == QString("copyright"));
		qDebug() << "Elapsed" << elapsed << "ms";
		REQUIRE(elapsed < 20);
	}
}
