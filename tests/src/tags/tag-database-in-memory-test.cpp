#include "tag-database-in-memory-test.h"
#include <QtTest>
#include "tags/tag.h"
#include "tags/tag-database-in-memory.h"


TagDatabaseInMemoryTest::TagDatabaseInMemoryTest()
	: TagDatabaseTestSuite(new TagDatabaseInMemory("tests/resources/tag-types.txt", "tests/resources/tags.txt"))
{}


void TagDatabaseInMemoryTest::loadNonExistingFile()
{
	QFile::remove("not_existing_tags_file.txt");

	TagDatabaseInMemory database("tests/resources/tag-types.txt", "not_existing_tags_file.txt");
	QVERIFY(database.load()); // Load should succeed even if the file does not exist

	QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");

	QCOMPARE(types.count(), 0);
}

void TagDatabaseInMemoryTest::loadEmpty()
{
	QTemporaryFile file;
	QVERIFY(file.open());

	TagDatabaseInMemory database("tests/resources/tag-types.txt", file.fileName());
	QVERIFY(database.load());

	QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");

	QCOMPARE(types.count(), 0);
	QCOMPARE(database.count(), 0);
}

void TagDatabaseInMemoryTest::loadInvalidLines()
{
	QTemporaryFile file;
	QVERIFY(file.open());
	file.write("tag1,1\ntag3\n");
	file.seek(0);

	TagDatabaseInMemory database("tests/resources/tag-types.txt", file.fileName());
	QVERIFY(database.load());

	QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");

	QCOMPARE(types.count(), 1);
	QCOMPARE(types.contains("tag1"), true);
	QCOMPARE(types.contains("tag3"), false);
	QCOMPARE(types.value("tag1").name(), QString("artist"));
	QCOMPARE(database.count(), 1);
}

void TagDatabaseInMemoryTest::loadValidData()
{
	QTemporaryFile file;
	QVERIFY(file.open());
	file.write("tag1,0\ntag2,1\ntag3,3\ntag4,4");
	file.seek(0);

	TagDatabaseInMemory database("tests/resources/tag-types.txt", file.fileName());
	QVERIFY(database.load());

	QMap<QString, TagType> types = database.getTagTypes(QStringList() << "tag1" << "tag3");

	QCOMPARE(types.count(), 2);
	QCOMPARE(types.contains("tag1"), true);
	QCOMPARE(types.contains("tag3"), true);
	QCOMPARE(types.value("tag1").name(), QString("general"));
	QCOMPARE(types.value("tag3").name(), QString("copyright"));
	QCOMPARE(database.count(), 4);
}


void TagDatabaseInMemoryTest::saveEmpty()
{
	QString filename = "test_tmp_tags_file.txt";

	TagDatabaseInMemory database("tests/resources/tag-types.txt", filename);
	QVERIFY(database.load());

	database.setTags(QList<Tag>());
	QVERIFY(database.save());

	QFile f(filename);
	QVERIFY(f.open(QFile::ReadOnly | QFile::Text));
	QString content = f.readAll();
	QVERIFY(content.isEmpty());
	QVERIFY(f.remove());
}

void TagDatabaseInMemoryTest::saveData()
{
	QString filename = "test_tmp_tags_file.txt";

	TagDatabaseInMemory database("tests/resources/tag-types.txt", filename);
	QVERIFY(database.load());

	QCOMPARE(database.count(), 0);
	database.setTags(QList<Tag>() << Tag("tag1", TagType("general")) << Tag("tag2", TagType("copyright")));
	QVERIFY(database.save());
	QCOMPARE(database.count(), 2);

	QFile f(filename);
	QVERIFY(f.open(QFile::ReadOnly | QFile::Text));
	QString content = f.readAll();
	QVERIFY(content.contains("tag1,0\n"));
	QVERIFY(content.contains("tag2,3\n"));
	QVERIFY(f.remove());
}


QTEST_MAIN(TagDatabaseInMemoryTest)
