#include <algorithm>
#include "functions.h"
#include "models/favorite.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "catch.h"


TEST_CASE("Favorite")
{
	SECTION("BasicConstructor")
	{
		Favorite fav("test");

		REQUIRE(fav.getName() == QString("test"));
		REQUIRE(fav.getNote() == 50);
		REQUIRE(fav.getLastViewed() > QDateTime::currentDateTime().addSecs(-60));
		REQUIRE(fav.getLastViewed() < QDateTime::currentDateTime().addSecs(60));
	}

	SECTION("GetName")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("fate/stay_night", 50, date);

		REQUIRE(fav.getName() == QString("fate/stay_night"));
	}
	SECTION("GetNameClean")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("fate/stay_night", 50, date);

		REQUIRE(fav.getName(true) == QString("fatestay_night"));
	}

	SECTION("GetNote")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("fate/stay_night", 50, date);

		REQUIRE(fav.getNote() == 50);
	}
	SECTION("SetNote")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("fate/stay_night", 50, date);
		fav.setNote(100);

		REQUIRE(fav.getNote() == 100);
	}

	SECTION("GetLastViewed")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("fate/stay_night", 50, date);

		REQUIRE(fav.getLastViewed() == date);
		REQUIRE(fav.getLastViewed().date().day() == 2);
	}
	SECTION("SetLastViewed")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("fate/stay_night", 50, QDateTime::currentDateTime());
		fav.setLastViewed(date);

		REQUIRE(fav.getLastViewed() == date);
	}

	SECTION("GetImagePath")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("fate/stay_night", 50, date, "test/test.jpg");

		REQUIRE(fav.getImagePath() == QString("test/test.jpg"));
	}
	SECTION("SetImagePath")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("fate/stay_night", 50, date, "test/test.jpg");
		fav.setImagePath("test/newimage.jpg");

		REQUIRE(fav.getImagePath() == QString("test/newimage.jpg"));
	}

	SECTION("GetMonitors")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Monitor monitor(nullptr, 60, date, false, "", "");
		Favorite fav("fate/stay_night", 50, date, QList<Monitor>() << monitor, "test/test.jpg");
		fav.setImagePath("test/newimage.jpg");

		REQUIRE(fav.getMonitors().count() == 1);
		REQUIRE(fav.getMonitors().first() == monitor);
	}

	SECTION("Equals")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav1("tag", 50, date);
		Favorite fav2("tag", 100, QDateTime::currentDateTime());

		REQUIRE(fav1 == fav2);
	}
	SECTION("EqualsAll")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav1("tag", 50, date);
		Favorite fav2("tag", 50, date);

		REQUIRE(fav1 == fav2);
	}
	SECTION("EqualsCase")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav1("tag", 50, date);
		Favorite fav2("TAg", 50, date);

		REQUIRE(fav1 == fav2);
	}

	SECTION("NotEquals")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav1("tag1", 50, date);
		Favorite fav2("tag2", 50, date);

		REQUIRE(fav1 != fav2);
	}

	#ifndef HEADLESS
	SECTION("SetImageFirst")
	{
		QFile file(savePath("thumbs/tag1.png"));
		if (file.exists())
			file.remove();

		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("tag1", 50, date);

		QPixmap img(QDir::currentPath() + "/tests/resources/image_200x200.png");
		fav.setImage(img);

		REQUIRE(file.exists() == true);
	}
	SECTION("GetImageNotExists")
	{
		QFile file(savePath("thumbs/tag1.png"));
		if (file.exists())
			file.remove();

		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("tag1", 50, date);

		QPixmap img = fav.getImage();

		REQUIRE(img.isNull() == true);
	}
	SECTION("GetImageBig")
	{
		QFile file(savePath("thumbs/tag1.png"));
		if (file.exists())
			file.remove();

		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("tag1", 50, date);

		QPixmap img(QDir::currentPath() + "/tests/resources/image_200x200.png");
		fav.setImage(img);
		QPixmap actual = fav.getImage();

		REQUIRE(actual.isNull() == false);
		REQUIRE(actual.size() == QSize(150, 150));
	}
	SECTION("GetImageSmall")
	{
		QFile file(savePath("thumbs/tag1.png"));
		if (file.exists())
			file.remove();

		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("tag1", 50, date);

		QPixmap img(QDir::currentPath() + "/tests/resources/image_1x1.png");
		fav.setImage(img);
		QPixmap actual = fav.getImage();

		REQUIRE(actual.isNull() == false);
		REQUIRE(actual.size() == QSize(150, 150));
	}
	SECTION("GetImageResize")
	{
		QFile file(savePath("thumbs/tag1.png"));
		if (file.exists())
			file.remove();

		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("tag1", 50, date, QDir::currentPath() + "/tests/resources/image_200x200.png");
		QPixmap actual = fav.getImage();

		REQUIRE(file.exists() == true);
		REQUIRE(actual.isNull() == false);
		REQUIRE(actual.size() == QSize(150, 150));
	}
	#endif

	SECTION("ToString")
	{
		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite fav("fate/stay_night", 50, date);

		REQUIRE(fav.toString() == QString("fate/stay_night|50|2016-07-02T16:35:12"));
	}

	SECTION("FromString")
	{
		QString from = "fate/stay_night|50|2016-07-02T16:35:12";

		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Favorite expected("fate/stay_night", 50, date);
		Favorite actual = Favorite::fromString("", from);

		REQUIRE(actual.getName() == expected.getName());
		REQUIRE(actual.getNote() == expected.getNote());
		REQUIRE(actual.getLastViewed() == expected.getLastViewed());
	}

	SECTION("SortByNote")
	{
		QList<Favorite> favorites =
		{
			Favorite("f1", 2, QDateTime(QDate(2018, 1, 3))),
			Favorite("f2", 3, QDateTime(QDate(2018, 1, 1))),
			Favorite("f3", 1, QDateTime(QDate(2018, 1, 2)))
		};

		std::sort(favorites.begin(), favorites.end(), Favorite::sortByNote);

		REQUIRE(favorites[0].getName() == QString("f3"));
		REQUIRE(favorites[1].getName() == QString("f1"));
		REQUIRE(favorites[2].getName() == QString("f2"));
	}

	SECTION("SortByName")
	{
		QList<Favorite> favorites =
		{
			Favorite("f1", 2, QDateTime(QDate(2018, 1, 3))),
			Favorite("f2", 3, QDateTime(QDate(2018, 1, 1))),
			Favorite("f3", 1, QDateTime(QDate(2018, 1, 2)))
		};

		std::sort(favorites.begin(), favorites.end(), Favorite::sortByName);

		REQUIRE(favorites[0].getName() == QString("f1"));
		REQUIRE(favorites[1].getName() == QString("f2"));
		REQUIRE(favorites[2].getName() == QString("f3"));
	}

	SECTION("SortByLastViewed")
	{
		QList<Favorite> favorites =
		{
			Favorite("f1", 2, QDateTime(QDate(2018, 1, 3))),
			Favorite("f2", 3, QDateTime(QDate(2018, 1, 1))),
			Favorite("f3", 1, QDateTime(QDate(2018, 1, 2)))
		};

		std::sort(favorites.begin(), favorites.end(), Favorite::sortByLastViewed);

		REQUIRE(favorites[0].getName() == QString("f2"));
		REQUIRE(favorites[1].getName() == QString("f3"));
		REQUIRE(favorites[2].getName() == QString("f1"));
	}

	SECTION("Serialization")
	{
		Profile profile("tests/resources/");
		Source source(&profile, "tests/resources/sites/Danbooru (2.0)");
		Site site("danbooru.donmai.us", &source);

		QDateTime date = QDateTime::fromString("2016-07-02 16:35:12", "yyyy-MM-dd HH:mm:ss");
		Monitor monitor(&site, 60, date, false, "", "");
		Favorite original("fate/stay_night", 50, date, QList<Monitor>() << monitor);

		QJsonObject json;
		original.toJson(json);

		Favorite dest = Favorite::fromJson("", json, &profile);

		REQUIRE(dest.getName() == original.getName());
		REQUIRE(dest.getNote() == original.getNote());
		REQUIRE(dest.getLastViewed() == original.getLastViewed());
	}
}
