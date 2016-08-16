#include <QtTest>
#include "favorite-test.h"


void FavoriteTest::testGetName()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav("fate/stay_night", 50, date);

	QCOMPARE(fav.getName(), QString("fate/stay_night"));
}
void FavoriteTest::testGetNameClean()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav("fate/stay_night", 50, date);

	QCOMPARE(fav.getName(true), QString("fatestay_night"));
}

void FavoriteTest::testGetNote()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav("fate/stay_night", 50, date);

	QCOMPARE(fav.getNote(), 50);
}
void FavoriteTest::testSetNote()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav("fate/stay_night", 50, date);
	fav.setNote(100);

	QCOMPARE(fav.getNote(), 100);
}

void FavoriteTest::testGetLastViewed()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav("fate/stay_night", 50, date);

	QCOMPARE(fav.getLastViewed(), date);
}
void FavoriteTest::testSetLastViewed()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav("fate/stay_night", 50, QDateTime::currentDateTime());
	fav.setLastViewed(date);

	QCOMPARE(fav.getLastViewed(), date);
}

void FavoriteTest::testGetImagePath()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav("fate/stay_night", 50, date, "test/test.jpg");

	QCOMPARE(fav.getImagePath(), QString("test/test.jpg"));
}
void FavoriteTest::testSetImagePath()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav("fate/stay_night", 50, date, "test/test.jpg");
	fav.setImagePath("test/newimage.jpg");

	QCOMPARE(fav.getImagePath(), QString("test/newimage.jpg"));
}

void FavoriteTest::testEquals()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav1("tag", 50, date);
	Favorite fav2("tag", 100, QDateTime::currentDateTime());

	QCOMPARE(true, fav1 == fav2);
}
void FavoriteTest::testEqualsAll()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav1("tag", 50, date);
	Favorite fav2("tag", 50, date);

	QCOMPARE(true, fav1 == fav2);
}
void FavoriteTest::testEqualsCase()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav1("tag", 50, date);
	Favorite fav2("TAg", 50, date);

	QCOMPARE(true, fav1 == fav2);
}

void FavoriteTest::testNotEquals()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav1("tag1", 50, date);
	Favorite fav2("tag2", 50, date);

	QCOMPARE(false, fav1 == fav2);
}

static FavoriteTest instance;
