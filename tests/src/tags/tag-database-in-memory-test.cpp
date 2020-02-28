#include "tags/tag.h"
#include "tags/tag-database-in-memory.h"
#include "catch.h"


// FIXME
/*TagDatabaseInMemoryTest::TagDatabaseInMemoryTest()
	: TagDatabaseTestSuite(new TagDatabaseInMemory("tests/resources/tag-types.txt", "tests/resources/tags.txt"))
{}


void TagDatabaseInMemoryTest::loadNonExistingFile()
{
	QFile::remove("not_existing_tags_file.txt");

	TagDatabaseInMemory database("tests/resources/tag-types.txt", "not_existing_tags_file.txt");
	REQUIRE(database.load()); // Load should succeed even if the file does not exist

	QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");

	REQUIRE(types.count() == 0);
}

void TagDatabaseInMemoryTest::loadEmpty()
{
	QTemporaryFile file;
	REQUIRE(file.open());

	TagDatabaseInMemory database("tests/resources/tag-types.txt", file.fileName());
	REQUIRE(database.load());

	QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");

	REQUIRE(types.count() == 0);
	REQUIRE(database.count() == 0);
}

SECTION("LoadInvalidTypes")
{
	QTemporaryFile file;
	REQUIRE(file.open());
	file.write("0,general\n1,artist\n2,invalid,test\n3,copyright\n4,character");
	file.seek(0);

	TagDatabaseInMemory database(file.fileName(), "tests/resources/tags.txt");
	REQUIRE(database.load());

	QMap<int, TagType> types = database.tagTypes();
	REQUIRE(types.count() == 4);
	REQUIRE(types.keys() == QList<int>() << 0 << 1 << 3 << 4);
	REQUIRE(types.value(0).name() == QString("general"));
	REQUIRE(types.value(1).name() == QString("artist"));
	REQUIRE(types.value(3).name() == QString("copyright"));
	REQUIRE(types.value(4).name() == QString("character"));
}

void TagDatabaseInMemoryTest::loadInvalidLines()
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

void TagDatabaseInMemoryTest::loadValidData()
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


void TagDatabaseInMemoryTest::saveEmpty()
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

void TagDatabaseInMemoryTest::saveData()
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


QTEST_MAIN(TagDatabaseInMemoryTest)*/
