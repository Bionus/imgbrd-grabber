#include <QtTest>
#include "tag-stylist-test.h"


void TagStylistTest::init()
{
	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);
}

void TagStylistTest::cleanup()
{
	m_settings->deleteLater();
}


void TagStylistTest::testBasic()
{
	m_settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/artists", "#aa0000");

	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QList<Favorite> favorites;
	favorites.append(Favorite("tag_other", 50, QDateTime::currentDateTime()));

	TagStylist stylist(new Profile(m_settings, favorites));
	QString actual = stylist.stylished(tag);
	QString expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(actual, expected);
}

void TagStylistTest::testIgnored()
{
	m_settings->setValue("Coloring/Fonts/ignoreds", ",8.25,-1,5,50,0,0,0,0,0");
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	Profile pro(m_settings, QList<Favorite>());
	pro.addIgnored("tag_text");

	TagStylist stylist(&pro);
	QString actual = stylist.stylished(tag);
	QString expected = "<a href=\"tag_text\" style=\"color:#999999; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(actual, expected);
}

void TagStylistTest::testBlacklisted()
{
	m_settings->setValue("Coloring/Fonts/blacklisteds", ",8.25,-1,5,50,0,0,0,0,0");
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	Profile pro(m_settings, QList<Favorite>());
	pro.addBlacklistedTag("tag_text");

	TagStylist stylist(&pro);
	QString actual = stylist.stylished(tag);
	QString expected = "<a href=\"tag_text\" style=\"color:#000000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(actual, expected);
}

void TagStylistTest::testFavorite()
{
	m_settings->setValue("Coloring/Fonts/favorites", ",8.25,-1,5,50,0,0,0,0,0");
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QList<Favorite> favorites;
	favorites.append(Favorite("tag_text", 50, QDateTime::currentDateTime()));

	TagStylist stylist(new Profile(m_settings, favorites));
	QString actual = stylist.stylished(tag);
	QString expected = "<a href=\"tag_text\" style=\"color:#ffc0cb; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(actual, expected);
}

void TagStylistTest::testKeptForLater()
{
	m_settings->setValue("Coloring/Fonts/keptForLater", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/keptForLater", "#aa0000");

	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QStringList keptForLater = QStringList() << "tag_text";

	TagStylist stylist(new Profile(m_settings, QList<Favorite>(), keptForLater));
	QString actual = stylist.stylished(tag);
	QString expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(actual, expected);
}

void TagStylistTest::testWithCount()
{
	m_settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/artists", "#aa0000");

	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	TagStylist stylist(new Profile(m_settings, QList<Favorite>()));
	QString actual = stylist.stylished(tag, true);
	QString expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a> <span style=\"color:#aaa\">(123)</span>";
	QCOMPARE(actual, expected);
}


static TagStylistTest instance;
