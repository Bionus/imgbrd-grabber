#include <QDebug>
#include <QTemporaryFile>
#include "tags/tag.h"
#include "tags/tag-database-in-memory.h"
#include "catch.h"


TEST_CASE("TagDatabaseInMemory")
{
	TagDatabaseInMemory database("tests/resources/tag-types.txt", "tests/resources/tags.txt");

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

	SECTION("Load non-existing file")
	{
		QFile::remove("not_existing_tags_file.txt");

		TagDatabaseInMemory database("tests/resources/tag-types.txt", "not_existing_tags_file.txt");
		REQUIRE(database.load()); // Load should succeed even if the file does not exist

		QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");

		REQUIRE(types.count() == 0);
	}

	SECTION("Load empty file")
	{
		QTemporaryFile file;
		REQUIRE(file.open());

		TagDatabaseInMemory database("tests/resources/tag-types.txt", file.fileName());
		REQUIRE(database.load());

		QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");

		REQUIRE(types.count() == 0);
		REQUIRE(database.count() == 0);
	}

	SECTION("Load invalid lines")
	{
		QTemporaryFile file;
		REQUIRE(file.open());
		file.write("tag1,1\ntag3\ntag4,123456\n,4");
		file.seek(0);

		TagDatabaseInMemory database("tests/resources/tag-types.txt", file.fileName());
		REQUIRE(database.load());

		QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");

		REQUIRE(types.count() == 1);
		REQUIRE(types.contains("tag1"));
		REQUIRE(!types.contains("tag3"));
		REQUIRE(!types.contains("tag4"));
		REQUIRE(types.value("tag1").name() == QString("artist"));
		REQUIRE(database.count() == 1);
	}

	SECTION("Load valid data")
	{
		QTemporaryFile file;
		REQUIRE(file.open());
		file.write("tag1,0\ntag2,1\ntag3,3\ntag4,4");
		file.seek(0);

		TagDatabaseInMemory database("tests/resources/tag-types.txt", file.fileName());
		REQUIRE(database.load());

		QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");

		REQUIRE(types.count() == 2);
		REQUIRE(types.contains("tag1"));
		REQUIRE(types.contains("tag3"));
		REQUIRE(types.value("tag1").name() == QString("general"));
		REQUIRE(types.value("tag3").name() == QString("copyright"));
		REQUIRE(database.count() == 4);
	}


	SECTION("Save empty")
	{
		QString filename = "test_tmp_tags_file.txt";

		TagDatabaseInMemory database("tests/resources/tag-types.txt", filename);
		REQUIRE(database.load());

		database.setTags(QList<Tag>());
		REQUIRE(database.save());

		QFile f(filename);
		REQUIRE(f.open(QFile::ReadOnly | QFile::Text));
		QString content = f.readAll();
		REQUIRE(content.isEmpty());
		REQUIRE(f.remove());
	}

	SECTION("Save data")
	{
		QString filename = "test_tmp_tags_file.txt";

		TagDatabaseInMemory database("tests/resources/tag-types.txt", filename);
		REQUIRE(database.load());

		REQUIRE(database.count() == 0);
		database.setTags(QList<Tag>() << Tag("tag1", TagType("general")) << Tag("tag2", TagType("copyright")));
		REQUIRE(database.save());
		REQUIRE(database.count() == 2);

		QFile f(filename);
		REQUIRE(f.open(QFile::ReadOnly | QFile::Text));
		QString content = f.readAll();
		REQUIRE(content.contains("tag1,0\n"));
		REQUIRE(content.contains("tag2,3\n"));
		REQUIRE(f.remove());
	}
}
