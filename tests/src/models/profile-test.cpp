#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPointer>
#include "models/profile.h"
#include "catch.h"
#include "source-helpers.h"


TEST_CASE("Profile")
{
	QList<QDateTime> dates
	{
		QDateTime(QDate(2016, 9, 1), QTime(9, 23, 17)),
		QDateTime(QDate(2016, 10, 1), QTime(12, 23, 17)),
		QDateTime(QDate(2016, 7, 1), QTime(9, 12, 17))
	};

	QFile::remove("tests/resources/favorites.json");
	QFile f("tests/resources/favorites.txt");
	f.open(QFile::WriteOnly | QFile::Text);
	f.write(Favorite("tag_1", 20, dates[0]).toString().toUtf8() + "\r\n");
	f.write(Favorite("tag_2", 100, dates[1]).toString().toUtf8() + "\r\n");
	f.close();

	QFile f2("tests/resources/md5s.txt");
	f2.open(QFile::WriteOnly | QFile::Text);
	f2.write(QString("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png\r\n").toUtf8());
	f2.write(QString("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png\r\n").toUtf8());
	f2.close();

	auto profile = QPointer<Profile>(makeProfile());

	SECTION("ConstructorEmpty")
	{
		Profile newProfile;

		REQUIRE(newProfile.getPath() == QString());
		REQUIRE(newProfile.getSettings() == nullptr);
		REQUIRE(newProfile.getFavorites().isEmpty());
		REQUIRE(newProfile.getKeptForLater().isEmpty());
	}

	SECTION("ConstructorPath")
	{
		REQUIRE(profile->getPath() == QString("tests/resources"));
		REQUIRE(profile->getSettings() != nullptr);
	}

	SECTION("LoadFavorites")
	{
		QList<Favorite> &favs = profile->getFavorites();

		REQUIRE(favs.count() == 2);
		REQUIRE(favs[0].getName() == QString("tag_1"));
		REQUIRE(favs[0].getNote() == 20);
		REQUIRE(favs[0].getLastViewed() == dates[0]);
		REQUIRE(favs[1].getName() == QString("tag_2"));
		REQUIRE(favs[1].getNote() == 100);
		REQUIRE(favs[1].getLastViewed() == dates[1]);
	}

	SECTION("AddFavorite")
	{
		Favorite fav("tag_3", 70, dates[2]);
		profile->addFavorite(fav);
		profile->sync();

		QFile f("tests/resources/favorites.json");
		f.open(QFile::ReadOnly | QFile::Text);
		QJsonObject json = QJsonDocument::fromJson(f.readAll()).object();
		QJsonArray lines = json["favorites"].toArray();
		f.close();

		REQUIRE(lines.count() == 3);
		REQUIRE(lines[0].toObject().value("tag").toString() == QString("tag_1"));
		REQUIRE(lines[1].toObject().value("tag").toString() == QString("tag_2"));
		REQUIRE(lines[2].toObject().value("tag").toString() == QString("tag_3"));
	}

	SECTION("RemoveFavorite")
	{
		profile->removeFavorite(Favorite("tag_1", 20, dates[0]));
		profile->sync();

		QFile f("tests/resources/favorites.json");
		f.open(QFile::ReadOnly | QFile::Text);
		QJsonObject json = QJsonDocument::fromJson(f.readAll()).object();
		QJsonArray lines = json["favorites"].toArray();
		f.close();

		REQUIRE(lines.count() == 1);
		REQUIRE(lines[0].toObject().value("tag").toString() == QString("tag_2"));
	}

	#ifndef Q_OS_WIN
	SECTION("RemoveFavoriteThumb")
	{
		Favorite fav("tag_1", 20, dates[0]);

		QDir(profile->getPath()).mkdir("thumb");
		QFile thumb(profile->getPath() + "/thumbs/" + fav.getName(true) + ".png");
		thumb.open(QFile::WriteOnly | QFile::Truncate);
		thumb.write(QString("test").toUtf8());
		thumb.close();

		REQUIRE(thumb.exists());
		profile->removeFavorite(fav);
		REQUIRE(!thumb.exists());
	}
	#endif
}
