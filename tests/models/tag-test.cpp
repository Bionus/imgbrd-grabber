#include <QtTest>
#include <QString>
#include "tag-test.h"


TagTest::TagTest()
{ }

void TagTest::init()
{
	m_settings = new QSettings("settings.ini", QSettings::IniFormat);
}
void TagTest::cleanup()
{
	m_settings->deleteLater();
}

void TagTest::testText()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.text(), QString("tag_text"));
}
void TagTest::testCount()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.count(), 123);
}
void TagTest::testType()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.type(), QString("artist"));
}
void TagTest::testShortType()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.shortType(), 1);
}
void TagTest::testStylishedFavorite()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QList<Favorite> favorites;
	favorites.append(Favorite("tag_text"));

	QCOMPARE(tag.stylished(favorites), QString("<span style=\"color:pink\">tag_text</span>"));
}
void TagTest::testStylishedNotFavorite()
{
	m_settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/artists", "#aa0000");

	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QList<Favorite> favorites;
	favorites.append(Favorite("tag_other"));

	QString expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(tag.stylished(favorites), expected);
}
void TagTest::testRelated()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.related(), QStringList() << "related1" << "related2" << "related3");
}
void TagTest::testTypedTextArtist()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.typedText(), QString("artist:tag_text"));
}
void TagTest::testTypedTextGeneral()
{
	Tag tag(m_settings, "tag_text", "general", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.typedText(), QString("tag_text"));
}
