#include <QtTest>
#include "tag-database-test-suite.h"


TagDatabaseTestSuite::TagDatabaseTestSuite(TagDatabase *database)
	: m_database(database)
{}


void TagDatabaseTestSuite::initTestCase()
{
	m_database->load();
	m_database->setTags(QList<Tag>());
}


void TagDatabaseTestSuite::testTypesProperlyLoaded()
{
	QMap<int, TagType> types = m_database->tagTypes();

	QCOMPARE(types.count(), 4);
	QCOMPARE(types.keys(), QList<int>() << 0 << 1 << 3 << 4);
	QCOMPARE(types.value(0).name(), QString("general"));
	QCOMPARE(types.value(1).name(), QString("artist"));
	QCOMPARE(types.value(3).name(), QString("copyright"));
	QCOMPARE(types.value(4).name(), QString("character"));
}

void TagDatabaseTestSuite::testEmptyContainsNone()
{
	m_database->setTags(QList<Tag>());

	QTime timer;
	timer.start();
	QMap<QString, TagType> types = m_database->getTagTypes(QStringList() << "tag1" << "tag3");
	int elapsed = timer.elapsed();

	QCOMPARE(types.count(), 0);
	qDebug() << "Elapsed" << elapsed << "ms";
	QVERIFY(elapsed < 10);
}

void TagDatabaseTestSuite::testFilledContainsAll()
{
	m_database->setTags(QList<Tag>() << Tag("tag1", TagType("general")) << Tag("tag2", TagType("artist")) << Tag("tag3", TagType("copyright")) << Tag("tag4", TagType("character")));

	QTime timer;
	timer.start();
	QMap<QString, TagType> types = m_database->getTagTypes(QStringList() << "tag1" << "tag3");
	int elapsed = timer.elapsed();

	QCOMPARE(types.count(), 2);
	QCOMPARE(types.contains("tag1"), true);
	QCOMPARE(types.contains("tag3"), true);
	QCOMPARE(types.value("tag1").name(), QString("general"));
	QCOMPARE(types.value("tag3").name(), QString("copyright"));
	qDebug() << "Elapsed" << elapsed << "ms";
	QVERIFY(elapsed < 10);
}

void TagDatabaseTestSuite::testFilledContainsSome()
{
	m_database->setTags(QList<Tag>() << Tag("tag1", TagType("general")) << Tag("tag2", TagType("artist")) << Tag("tag3", TagType("copyright")) << Tag("tag4", TagType("character")));

	QTime timer;
	timer.start();
	QMap<QString, TagType> types = m_database->getTagTypes(QStringList() << "tag1" << "tag3" << "tag5" << "missing_tag");
	int elapsed = timer.elapsed();

	QCOMPARE(types.count(), 2);
	QCOMPARE(types.contains("tag1"), true);
	QCOMPARE(types.contains("tag3"), true);
	QCOMPARE(types.contains("tag5"), false);
	QCOMPARE(types.contains("missing_tag"), false);
	QCOMPARE(types.value("tag1").name(), QString("general"));
	QCOMPARE(types.value("tag3").name(), QString("copyright"));
	qDebug() << "Elapsed" << elapsed << "ms";
	QVERIFY(elapsed < 10);
}
