#include <QtTest>
#include "favorite-test.h"
#include "functions.h"


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


static FavoriteTest instance;
