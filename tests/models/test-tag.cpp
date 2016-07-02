#include <QtTest>
#include <QString>
#include "test-tag.h"


TestTag::TestTag()
{ }

void TestTag::init()
{
	m_settings = new QSettings("settings.ini", QSettings::IniFormat);
}
void TestTag::cleanup()
{
	m_settings->deleteLater();
}

void TestTag::testText()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.text(), QString("tag_text"));
}
void TestTag::testCount()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.count(), 123);
}
void TestTag::testType()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.type(), QString("artist"));
}
void TestTag::testShortType()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.shortType(), 1);
}
void TestTag::testStylishedFavorite()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QList<Favorite> favorites;
	favorites.append(Favorite("tag_text"));

	QCOMPARE(tag.stylished(favorites), QString("<span style=\"color:pink\">tag_text</span>"));
}
void TestTag::testStylishedNotFavorite()
{
	m_settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/artists", "#aa0000");

	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QList<Favorite> favorites;
	favorites.append(Favorite("tag_other"));

	QString expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(tag.stylished(favorites), expected);
}
void TestTag::testRelated()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.related(), QStringList() << "related1" << "related2" << "related3");
}
void TestTag::testTypedTextArtist()
{
	Tag tag(m_settings, "tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.typedText(), QString("artist:tag_text"));
}
void TestTag::testTypedTextGeneral()
{
	Tag tag(m_settings, "tag_text", "general", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.typedText(), QString("tag_text"));
}
