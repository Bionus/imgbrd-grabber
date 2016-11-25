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

	QCOMPARE(tag.type(), QString("artist"));
}
void TagTest::testTypeArtistEnding()
{
	Tag tag("tag_text (artist)", "unknown", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.type(), QString("artist"));
	QCOMPARE(tag.text(), QString("tag_text"));
}
void TagTest::testTypePrefix()
{
	Tag tag("artist:tag_text", "unknown", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.type(), QString("artist"));
	QCOMPARE(tag.text(), QString("tag_text"));
}
void TagTest::testTypePrefixSpecies()
{
	Tag tag("species:tag_text", "unknown", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.type(), QString("species"));
	QCOMPARE(tag.text(), QString("tag_text"));
}
void TagTest::testSetType()
{
	Tag tag("tag_text", "character", 123, QStringList() << "related1" << "related2" << "related3");
	tag.setType("artist");

	QCOMPARE(tag.type(), QString("artist"));
}
void TagTest::testShortType()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.shortType(), 1);
}
void TagTest::testSetTypeShortType()
{
	Tag tag("tag_text", "character", 123, QStringList() << "related1" << "related2" << "related3");
	tag.setType("artist");

	QCOMPARE(tag.shortType(), 1);
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

void TagTest::testTypedTextArtist()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.typedText(), QString("artist:tag_text"));
}
void TagTest::testTypedTextSpecies()
{
	Tag tag("tag_text", "species", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.typedText(), QString("species:tag_text"));
}

void TagTest::testTypedTextGeneral()
{
	Tag tag("tag_text", "general", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.typedText(), QString("tag_text"));
}

void TagTest::testStylishedBasic()
{
	m_settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/artists", "#aa0000");

	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QList<Favorite> favorites;
	favorites.append(Favorite("tag_other", 50, QDateTime::currentDateTime()));

	m_profile = new Profile(m_settings, favorites);
	QString actual = tag.stylished(m_profile);
	QString expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(actual, expected);
}
void TagTest::testStylishedIgnored()
{
	m_settings->setValue("Coloring/Fonts/ignoreds", ",8.25,-1,5,50,0,0,0,0,0");
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	m_profile = new Profile(m_settings, QList<Favorite>());
	QString actual = tag.stylished(m_profile, QStringList() << "tag_text");
	QString expected = "<a href=\"tag_text\" style=\"color:#999999; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(actual, expected);
}
void TagTest::testStylishedBlacklisted()
{
	m_settings->setValue("Coloring/Fonts/blacklisteds", ",8.25,-1,5,50,0,0,0,0,0");
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	m_profile = new Profile(m_settings, QList<Favorite>());
	QString actual = tag.stylished(m_profile, QStringList(), QStringList() << "tag_text");
	QString expected = "<a href=\"tag_text\" style=\"color:#000000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(actual, expected);
}
void TagTest::testStylishedFavorite()
{
	m_settings->setValue("Coloring/Fonts/favorites", ",8.25,-1,5,50,0,0,0,0,0");
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QList<Favorite> favorites;
	favorites.append(Favorite("tag_text", 50, QDateTime::currentDateTime()));

	m_profile = new Profile(m_settings, favorites);
	QString actual = tag.stylished(m_profile);
	QString expected = "<a href=\"tag_text\" style=\"color:#ffc0cb; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(actual, expected);
}
void TagTest::testStylishedWithCount()
{
	m_settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/artists", "#aa0000");

	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	m_profile = new Profile(m_settings, QList<Favorite>());
	QString actual = tag.stylished(m_profile, QStringList(), QStringList(), true);
	QString expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a> <span style=\"color:#aaa\">(123)</span>";
	QCOMPARE(actual, expected);
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

void TagTest::testSortByFrequency()
{
	QList<Tag> taglist;
	taglist.append(Tag("last", "artist", 1, QStringList() << "tag1"));
	taglist.append(Tag("fourth", "general", 2, QStringList() << "tag2"));
	taglist.append(Tag("third", "copyright", 3, QStringList() << "tag3"));
	taglist.append(Tag("second", "character", 4, QStringList() << "tag4"));
	taglist.append(Tag("first", "unknown", 5, QStringList() << "tag5"));

	qSort(taglist.begin(), taglist.end(), sortByFrequency);

	QCOMPARE(taglist[0].text(), QString("first"));
	QCOMPARE(taglist[1].text(), QString("second"));
	QCOMPARE(taglist[2].text(), QString("third"));
	QCOMPARE(taglist[3].text(), QString("fourth"));
	QCOMPARE(taglist[4].text(), QString("last"));
}

void TagTest::testTypeSpaced()
{
	Tag tag("artist1", "artist with spaces", 1, QStringList() << "tag1");

	QCOMPARE(tag.type(), QString("artist"));
}

static TagTest instance;
