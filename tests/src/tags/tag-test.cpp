#include <QtTest>
#include "tag-test.h"


void TagTest::init()
{
	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);
}
void TagTest::cleanup()
{
	m_settings->deleteLater();
}

void TagTest::testDefaultConstructor()
{
	Tag tag;

	QCOMPARE(tag.text(), QString());
}

void TagTest::testText()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.text(), QString("tag_text"));
}
void TagTest::testSetText()
{
	Tag tag("tag_not_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	tag.setText("tag_text");

	QCOMPARE(tag.text(), QString("tag_text"));
}

void TagTest::testType()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.type().name(), QString("artist"));
}
void TagTest::testTypeArtistEnding()
{
	Tag tag("tag_text (artist)", "unknown", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.type().name(), QString("artist"));
	QCOMPARE(tag.text(), QString("tag_text"));
}
void TagTest::testTypePrefix()
{
	Tag tag("artist:tag_text", "unknown", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.type().name(), QString("artist"));
	QCOMPARE(tag.text(), QString("tag_text"));
}
void TagTest::testTypePrefixSpecies()
{
	Tag tag("species:tag_text", "unknown", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.type().name(), QString("species"));
	QCOMPARE(tag.text(), QString("tag_text"));
}
void TagTest::testShortType()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.type().number(), 1);
}

void TagTest::testCount()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.count(), 123);
}
void TagTest::testSetCount()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	tag.setCount(1234);

	QCOMPARE(tag.count(), 1234);
}

void TagTest::testRelated()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.related(), QStringList() << "related1" << "related2" << "related3");
}
void TagTest::testSetRelated()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2");
	tag.setRelated(QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.related(), QStringList() << "related1" << "related2" << "related3");
}

void TagTest::testCompare()
{
	Tag tag1("artist1", "artist", 1, QStringList() << "tag1");
	Tag tag2("artist1", "artist", 2, QStringList() << "tag2");
	Tag tag3("artist2", "artist", 3, QStringList() << "tag3");
	Tag tag4("artist1", "character", 4, QStringList() << "tag4");
	Tag tag5("artist1", "unknown", 5, QStringList() << "tag5");

	QCOMPARE(tag1 == tag1, true);
	QCOMPARE(tag1 == tag2, true);
	QCOMPARE(tag1 == tag3, false);
	QCOMPARE(tag1 == tag4, false);
	QCOMPARE(tag1 == tag5, true);
}

void TagTest::testSortTagsByType()
{
	QList<Tag> tagList;
	tagList.append(Tag("last", "artist", 1, QStringList() << "tag1"));
	tagList.append(Tag("fourth", "general", 2, QStringList() << "tag2"));
	tagList.append(Tag("third", "copyright", 3, QStringList() << "tag3"));
	tagList.append(Tag("second", "character", 4, QStringList() << "tag4"));
	tagList.append(Tag("first", "unknown", 5, QStringList() << "tag5"));

	qSort(tagList.begin(), tagList.end(), sortTagsByType);

	QCOMPARE(tagList[0].text(), QString("third"));
	QCOMPARE(tagList[1].text(), QString("second"));
	QCOMPARE(tagList[2].text(), QString("last"));
	QCOMPARE(tagList[3].text(), QString("first"));
	QCOMPARE(tagList[4].text(), QString("fourth"));
}
void TagTest::testSortTagsByName()
{
	QList<Tag> tagList;
	tagList.append(Tag("last", "artist", 1, QStringList() << "tag1"));
	tagList.append(Tag("fourth", "general", 2, QStringList() << "tag2"));
	tagList.append(Tag("third", "copyright", 3, QStringList() << "tag3"));
	tagList.append(Tag("second", "character", 4, QStringList() << "tag4"));
	tagList.append(Tag("first", "unknown", 5, QStringList() << "tag5"));

	qSort(tagList.begin(), tagList.end(), sortTagsByName);

	QCOMPARE(tagList[0].text(), QString("first"));
	QCOMPARE(tagList[1].text(), QString("fourth"));
	QCOMPARE(tagList[2].text(), QString("last"));
	QCOMPARE(tagList[3].text(), QString("second"));
	QCOMPARE(tagList[4].text(), QString("third"));
}
void TagTest::testSortTagsByCount()
{
	QList<Tag> tagList;
	tagList.append(Tag("last", "artist", 1, QStringList() << "tag1"));
	tagList.append(Tag("fourth", "general", 2, QStringList() << "tag2"));
	tagList.append(Tag("third", "copyright", 3, QStringList() << "tag3"));
	tagList.append(Tag("second", "character", 4, QStringList() << "tag4"));
	tagList.append(Tag("first", "unknown", 5, QStringList() << "tag5"));

	qSort(tagList.begin(), tagList.end(), sortTagsByCount);

	QCOMPARE(tagList[0].text(), QString("first"));
	QCOMPARE(tagList[1].text(), QString("second"));
	QCOMPARE(tagList[2].text(), QString("third"));
	QCOMPARE(tagList[3].text(), QString("fourth"));
	QCOMPARE(tagList[4].text(), QString("last"));
}

void TagTest::testTypeSpaced()
{
	Tag tag("artist1", "artist with spaces", 1, QStringList() << "tag1");

	QCOMPARE(tag.type().name(), QString("artist"));
}

void TagTest::testGetType()
{
	QStringList ids = QStringList() << "type1" << "type2" << "type3";

	// Basic types
	QCOMPARE(Tag::GetType("copyright", ids), QString("copyright"));
	QCOMPARE(Tag::GetType("artist", ids), QString("artist"));

	// Type IDs
	QCOMPARE(Tag::GetType("0", ids), QString("type1"));
	QCOMPARE(Tag::GetType("1", ids), QString("type2"));
	QCOMPARE(Tag::GetType("5", ids), QString("5"));
	QCOMPARE(Tag::GetType("-1", ids), QString("-1"));

	// Replacements
	QCOMPARE(Tag::GetType("series", ids), QString("copyright"));
	QCOMPARE(Tag::GetType("mangaka", ids), QString("artist"));
	QCOMPARE(Tag::GetType("game", ids), QString("copyright"));
	QCOMPARE(Tag::GetType("studio", ids), QString("circle"));
	QCOMPARE(Tag::GetType("source", ids), QString("general"));
	QCOMPARE(Tag::GetType("character group", ids), QString("general"));

	// Mixed types
	QCOMPARE(Tag::GetType("copyright, character", ids), QString("copyright"));
}


static TagTest instance;
