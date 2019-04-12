#include "tags/tag.h"
#include "tags/tag-database.h"
#include "catch.h"


// FIXME
/*TagDatabaseTestSuite::TagDatabaseTestSuite(TagDatabase *database)
	: m_database(database)
{}


void TagDatabaseTestSuite::initTestCase()
{
	m_database->open();
	m_database->load();
	m_database->setTags(QList<Tag>());
}


void TagDatabaseTestSuite::testAlreadyLoaded()
{
	m_database->load();

	REQUIRE(m_database->tagTypes().count() == 4);
}

void TagDatabaseTestSuite::testTypesProperlyLoaded()
{
	QMap<int, TagType> types = m_database->tagTypes();

	REQUIRE(types.count() == 4);
	REQUIRE(types.keys() == QList<int>() << 0 << 1 << 3 << 4);
	REQUIRE(types.value(0).name() == QString("general"));
	REQUIRE(types.value(1).name() == QString("artist"));
	REQUIRE(types.value(3).name() == QString("copyright"));
	REQUIRE(types.value(4).name() == QString("character"));
}

void TagDatabaseTestSuite::testEmptyContainsNone()
{
	m_database->setTags(QList<Tag>());

	QTime timer;
	timer.start();
	QMap<QString, TagType> types = m_database->getTagTypes(QStringList() << "tag1" << "tag3");
	int elapsed = timer.elapsed();

	REQUIRE(types.count() == 0);
	qDebug() << "Elapsed" << elapsed << "ms";
	REQUIRE(elapsed < 20);

	REQUIRE(m_database->count() == 0);
}

void TagDatabaseTestSuite::testFilledContainsAll()
{
	m_database->setTags(QList<Tag>() << Tag("tag1", TagType("general")) << Tag("tag2", TagType("artist")) << Tag("tag3", TagType("copyright")) << Tag("tag4", TagType("character")));

	QTime timer;
	timer.start();
	QMap<QString, TagType> types = m_database->getTagTypes(QStringList() << "tag1" << "tag3");
	int elapsed = timer.elapsed();

	REQUIRE(types.count() == 2);
	REQUIRE(types.contains("tag1") == true);
	REQUIRE(types.contains("tag3") == true);
	REQUIRE(types.value("tag1").name() == QString("general"));
	REQUIRE(types.value("tag3").name() == QString("copyright"));
	qDebug() << "Elapsed" << elapsed << "ms";
	REQUIRE(elapsed < 20);

	REQUIRE(m_database->count() == 4);
}

void TagDatabaseTestSuite::testFilledContainsSome()
{
	m_database->setTags(QList<Tag>() << Tag("tag1", TagType("general")) << Tag("tag2", TagType("artist")) << Tag("tag3", TagType("copyright")) << Tag("tag4", TagType("character")));

	QTime timer;
	timer.start();
	QMap<QString, TagType> types = m_database->getTagTypes(QStringList() << "tag1" << "tag3" << "tag5" << "missing_tag");
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
}*/
