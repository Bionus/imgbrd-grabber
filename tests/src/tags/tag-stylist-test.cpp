#include "tag-stylist-test.h"
#include <QtTest>
#include "tags/tag.h"
#include "tags/tag-stylist.h"


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
	QString actual = stylist.stylished(QList<Tag>() << tag).join("");
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
	QString actual = stylist.stylished(QList<Tag>() << tag).join("");
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
	QString actual = stylist.stylished(QList<Tag>() << tag).join("");
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
	QString actual = stylist.stylished(QList<Tag>() << tag).join("");
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
	QString actual = stylist.stylished(QList<Tag>() << tag).join("");
	QString expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a>";
	QCOMPARE(actual, expected);
}

void TagStylistTest::testWithCount()
{
	m_settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/artists", "#aa0000");

	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	TagStylist stylist(new Profile(m_settings, QList<Favorite>()));
	QString actual = stylist.stylished(QList<Tag>() << tag, true).join("");
	QString expected = "<a href=\"tag_text\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag_text</a> <span style=\"color:#aaa\">(123)</span>";
	QCOMPARE(actual, expected);
}

void TagStylistTest::testSortName()
{
	assertSort("name", QStringList() << "tag1" << "tag2" << "tag3");
}
void TagStylistTest::testSortType()
{
	assertSort("type", QStringList() << "tag2" << "tag1" << "tag3");
}
void TagStylistTest::testSortCount()
{
	assertSort("count", QStringList() << "tag3" << "tag2" << "tag1");
}


void TagStylistTest::assertSort(const QString &sort, const QStringList &expectedOrder)
{
	m_settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/artists", "#aa0000");
	m_settings->setValue("Coloring/Fonts/generals", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/generals", "#aa0000");
	m_settings->setValue("Coloring/Fonts/copyrights", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Colors/copyrights", "#aa0000");

	auto tags = QList<Tag>
	{
		Tag("tag3", "general", 3, QStringList()),
		Tag("tag2", "copyright", 2, QStringList()),
		Tag("tag1", "artist", 1, QStringList()),
	};

	TagStylist stylist(new Profile(m_settings, QList<Favorite>()));
	QStringList actual = stylist.stylished(tags, false, false, sort);

	QString format = "<a href=\"%1\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">%1</a>";
	QStringList expected;
	for (const QString &tag : expectedOrder)
		expected.append(format.arg(tag));

	QCOMPARE(actual, expected);
}


QTEST_MAIN(TagStylistTest)
