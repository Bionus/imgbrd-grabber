#include "favorite-test.h"
#include <QtTest>
#include <algorithm>
#include "functions.h"
#include "models/favorite.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


void FavoriteTest::testBasicConstructor()
{
	Favorite fav("test");

	QCOMPARE(fav.getName(), QString("test"));
	QCOMPARE(fav.getNote(), 50);
	QVERIFY(fav.getLastViewed() > QDateTime::currentDateTime().addSecs(-60) && fav.getLastViewed() < QDateTime::currentDateTime().addSecs(60));
}

void FavoriteTest::testGetName()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("fate/stay_night", 50, date);

	QCOMPARE(fav.getName(), QString("fate/stay_night"));
}
void FavoriteTest::testGetNameClean()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("fate/stay_night", 50, date);

	QCOMPARE(fav.getName(true), QString("fatestay_night"));
}

void FavoriteTest::testGetNote()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("fate/stay_night", 50, date);

	QCOMPARE(fav.getNote(), 50);
}
void FavoriteTest::testSetNote()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("fate/stay_night", 50, date);
	fav.setNote(100);

	QCOMPARE(fav.getNote(), 100);
}

void FavoriteTest::testGetLastViewed()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("fate/stay_night", 50, date);

	QCOMPARE(fav.getLastViewed(), date);
	QCOMPARE(fav.getLastViewed().date().day(), 2);
}
void FavoriteTest::testSetLastViewed()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("fate/stay_night", 50, QDateTime::currentDateTime());
	fav.setLastViewed(date);

	QCOMPARE(fav.getLastViewed(), date);
}

void FavoriteTest::testGetImagePath()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("fate/stay_night", 50, date, "test/test.jpg");

	QCOMPARE(fav.getImagePath(), QString("test/test.jpg"));
}
void FavoriteTest::testSetImagePath()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("fate/stay_night", 50, date, "test/test.jpg");
	fav.setImagePath("test/newimage.jpg");

	QCOMPARE(fav.getImagePath(), QString("test/newimage.jpg"));
}

void FavoriteTest::testGetMonitors()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
    Monitor monitor(nullptr, 60, date, false, "", "");
	Favorite fav("fate/stay_night", 50, date, QList<Monitor>() << monitor, "test/test.jpg");
	fav.setImagePath("test/newimage.jpg");

	QCOMPARE(fav.getMonitors().count(), 1);
	QCOMPARE(fav.getMonitors().first(), monitor);
}

void FavoriteTest::testEquals()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav1("tag", 50, date);
	Favorite fav2("tag", 100, QDateTime::currentDateTime());

	QCOMPARE(true, fav1 == fav2);
	QCOMPARE(false, fav1 != fav2);
}
void FavoriteTest::testEqualsAll()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav1("tag", 50, date);
	Favorite fav2("tag", 50, date);

	QCOMPARE(true, fav1 == fav2);
	QCOMPARE(false, fav1 != fav2);
}
void FavoriteTest::testEqualsCase()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav1("tag", 50, date);
	Favorite fav2("TAg", 50, date);

	QCOMPARE(true, fav1 == fav2);
	QCOMPARE(false, fav1 != fav2);
}

void FavoriteTest::testNotEquals()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav1("tag1", 50, date);
	Favorite fav2("tag2", 50, date);

	QCOMPARE(false, fav1 == fav2);
	QCOMPARE(true, fav1 != fav2);
}

#ifndef HEADLESS
void FavoriteTest::testSetImageFirst()
{
	QFile file(savePath("thumbs/tag1.png"));
	if (file.exists())
		file.remove();

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("tag1", 50, date);

	QPixmap img(QDir::currentPath() + "/tests/resources/image_200x200.png");
	fav.setImage(img);

	QCOMPARE(file.exists(), true);
}
void FavoriteTest::testGetImageNotExists()
{
	QFile file(savePath("thumbs/tag1.png"));
	if (file.exists())
		file.remove();

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("tag1", 50, date);

	QPixmap img = fav.getImage();

	QCOMPARE(img.isNull(), true);
}
void FavoriteTest::testGetImageBig()
{
	QFile file(savePath("thumbs/tag1.png"));
	if (file.exists())
		file.remove();

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("tag1", 50, date);

	QPixmap img(QDir::currentPath() + "/tests/resources/image_200x200.png");
	fav.setImage(img);
	QPixmap actual = fav.getImage();

	QCOMPARE(actual.isNull(), false);
	QCOMPARE(actual.size(), QSize(150, 150));
}
void FavoriteTest::testGetImageSmall()
{
	QFile file(savePath("thumbs/tag1.png"));
	if (file.exists())
		file.remove();

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("tag1", 50, date);

	QPixmap img(QDir::currentPath() + "/tests/resources/image_1x1.png");
	fav.setImage(img);
	QPixmap actual = fav.getImage();

	QCOMPARE(actual.isNull(), false);
	QCOMPARE(actual.size(), QSize(150, 150));
}
void FavoriteTest::testGetImageResize()
{
	QFile file(savePath("thumbs/tag1.png"));
	if (file.exists())
		file.remove();

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("tag1", 50, date, QDir::currentPath() + "/tests/resources/image_200x200.png");
	QPixmap actual = fav.getImage();

	QCOMPARE(file.exists(), true);
	QCOMPARE(actual.isNull(), false);
	QCOMPARE(actual.size(), QSize(150, 150));
}
#endif

void FavoriteTest::testToString()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite fav("fate/stay_night", 50, date);

	QCOMPARE(fav.toString(), QString("fate/stay_night|50|2016-07-02T16:35:12"));
}

void FavoriteTest::testFromString()
{
	QString from = "fate/stay_night|50|2016-07-02T16:35:12";

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
	Favorite expected("fate/stay_night", 50, date);
	Favorite actual = Favorite::fromString("", from);

	QCOMPARE(actual.getName(), expected.getName());
	QCOMPARE(actual.getNote(), expected.getNote());
	QCOMPARE(actual.getLastViewed(), expected.getLastViewed());
}

void FavoriteTest::testSortByNote()
{
	QList<Favorite> favorites =
	{
		Favorite("f1", 2, QDateTime(QDate(2018, 1, 3))),
		Favorite("f2", 3, QDateTime(QDate(2018, 1, 1))),
		Favorite("f3", 1, QDateTime(QDate(2018, 1, 2)))
	};

	std::sort(favorites.begin(), favorites.end(), Favorite::sortByNote);

	QCOMPARE(favorites[0].getName(), QString("f3"));
	QCOMPARE(favorites[1].getName(), QString("f1"));
	QCOMPARE(favorites[2].getName(), QString("f2"));
}

void FavoriteTest::testSortByName()
{
	QList<Favorite> favorites =
	{
		Favorite("f1", 2, QDateTime(QDate(2018, 1, 3))),
		Favorite("f2", 3, QDateTime(QDate(2018, 1, 1))),
		Favorite("f3", 1, QDateTime(QDate(2018, 1, 2)))
	};

	std::sort(favorites.begin(), favorites.end(), Favorite::sortByName);

	QCOMPARE(favorites[0].getName(), QString("f1"));
	QCOMPARE(favorites[1].getName(), QString("f2"));
	QCOMPARE(favorites[2].getName(), QString("f3"));
}

void FavoriteTest::testSortByLastViewed()
{
	QList<Favorite> favorites =
	{
		Favorite("f1", 2, QDateTime(QDate(2018, 1, 3))),
		Favorite("f2", 3, QDateTime(QDate(2018, 1, 1))),
		Favorite("f3", 1, QDateTime(QDate(2018, 1, 2)))
	};

	std::sort(favorites.begin(), favorites.end(), Favorite::sortByLastViewed);

	QCOMPARE(favorites[0].getName(), QString("f2"));
	QCOMPARE(favorites[1].getName(), QString("f3"));
	QCOMPARE(favorites[2].getName(), QString("f1"));
}

void FavoriteTest::testSerialization()
{
	Profile profile("tests/resources/");
	Source source(&profile, "tests/resources/sites/Danbooru (2.0)");
	Site site("danbooru.donmai.us", &source);

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
    Monitor monitor(&site, 60, date, false, "", "");
	Favorite original("fate/stay_night", 50, date, QList<Monitor>() << monitor);

	QJsonObject json;
	original.toJson(json);

	Favorite dest = Favorite::fromJson("", json, QMap<QString, Site*> {{ site.url(), &site }});

	QCOMPARE(dest.getName(), original.getName());
	QCOMPARE(dest.getNote(), original.getNote());
	QCOMPARE(dest.getLastViewed(), original.getLastViewed());
}


QTEST_MAIN(FavoriteTest)
