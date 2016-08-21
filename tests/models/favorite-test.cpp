#include <QtTest>
#include "favorite-test.h"
#include "functions.h"


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
	QCOMPARE(false, fav1 != fav2);
}
void FavoriteTest::testEqualsAll()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav1("tag", 50, date);
	Favorite fav2("tag", 50, date);

	QCOMPARE(true, fav1 == fav2);
	QCOMPARE(false, fav1 != fav2);
}
void FavoriteTest::testEqualsCase()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav1("tag", 50, date);
	Favorite fav2("TAg", 50, date);

	QCOMPARE(true, fav1 == fav2);
	QCOMPARE(false, fav1 != fav2);
}

void FavoriteTest::testNotEquals()
{
	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav1("tag1", 50, date);
	Favorite fav2("tag2", 50, date);

	QCOMPARE(false, fav1 == fav2);
	QCOMPARE(true, fav1 != fav2);
}

void FavoriteTest::testSetImageFirst()
{
	QFile file(savePath("thumbs/tag1.png"));
	if (file.exists())
		file.remove();

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
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

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav("tag1", 50, date);

	QPixmap img = fav.getImage();

	QCOMPARE(img.isNull(), true);
}
void FavoriteTest::testGetImageBig()
{
	QFile file(savePath("thumbs/tag1.png"));
	if (file.exists())
		file.remove();

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
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

	QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:sss");
	Favorite fav("tag1", 50, date);

	QPixmap img(QDir::currentPath() + "/tests/resources/image_1x1.png");
	fav.setImage(img);
	QPixmap actual = fav.getImage();

	QCOMPARE(actual.isNull(), false);
	QCOMPARE(actual.size(), QSize(150, 150));
}

static FavoriteTest instance;
