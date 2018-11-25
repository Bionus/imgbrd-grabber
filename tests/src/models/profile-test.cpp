#include "profile-test.h"
#include <QFile>
#include <QtTest>
#include "models/profile.h"


void ProfileTest::init()
{
	m_dates.clear();
	m_dates.append(QDateTime(QDate(2016, 9, 1), QTime(9, 23, 17)));
	m_dates.append(QDateTime(QDate(2016, 10, 1), QTime(12, 23, 17)));
	m_dates.append(QDateTime(QDate(2016, 7, 1), QTime(9, 12, 17)));

	QFile::remove("tests/resources/favorites.json");
	QFile f("tests/resources/favorites.txt");
	f.open(QFile::WriteOnly | QFile::Text);
	f.write(Favorite("tag_1", 20, m_dates[0]).toString().toUtf8() + "\r\n");
	f.write(Favorite("tag_2", 100, m_dates[1]).toString().toUtf8() + "\r\n");
	f.close();

	QFile f2("tests/resources/md5s.txt");
	f2.open(QFile::WriteOnly | QFile::Text);
	f2.write(QString("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png\r\n").toUtf8());
	f2.write(QString("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png\r\n").toUtf8());
	f2.close();

	m_profile = new Profile("tests/resources/");
}

void ProfileTest::cleanup()
{
	delete m_profile;
}


void ProfileTest::testConstructorEmpty()
{
	Profile newProfile;

	QCOMPARE(newProfile.getPath(), QString());
	QVERIFY(newProfile.getSettings() == nullptr);
	QVERIFY(newProfile.getFavorites().isEmpty());
	QVERIFY(newProfile.getKeptForLater().isEmpty());
}

void ProfileTest::testConstructorPath()
{
	QCOMPARE(m_profile->getPath(), QString("tests/resources/"));
	QVERIFY(m_profile->getSettings() != nullptr);
}

void ProfileTest::testLoadFavorites()
{
	QList<Favorite> &favs = m_profile->getFavorites();

	QCOMPARE(favs.count(), 2);
	QCOMPARE(favs[0].getName(), QString("tag_1"));
	QCOMPARE(favs[0].getNote(), 20);
	QCOMPARE(favs[0].getLastViewed(), m_dates[0]);
	QCOMPARE(favs[1].getName(), QString("tag_2"));
	QCOMPARE(favs[1].getNote(), 100);
	QCOMPARE(favs[1].getLastViewed(), m_dates[1]);
}

void ProfileTest::testAddFavorite()
{
	Favorite fav("tag_3", 70, m_dates[2]);
	m_profile->addFavorite(fav);
	m_profile->sync();

	QFile f("tests/resources/favorites.json");
	f.open(QFile::ReadOnly | QFile::Text);
	QJsonObject json = QJsonDocument::fromJson(f.readAll()).object();
	QJsonArray lines = json["favorites"].toArray();
	f.close();

	QCOMPARE(lines.count(), 3);
	QCOMPARE(lines[0].toObject().value("tag").toString(), QString("tag_1"));
	QCOMPARE(lines[1].toObject().value("tag").toString(), QString("tag_2"));
	QCOMPARE(lines[2].toObject().value("tag").toString(), QString("tag_3"));
}

void ProfileTest::testRemoveFavorite()
{
	m_profile->removeFavorite(Favorite("tag_1", 20, m_dates[0]));
	m_profile->sync();

	QFile f("tests/resources/favorites.json");
	f.open(QFile::ReadOnly | QFile::Text);
	QJsonObject json = QJsonDocument::fromJson(f.readAll()).object();
	QJsonArray lines = json["favorites"].toArray();
	f.close();

	QCOMPARE(lines.count(), 1);
	QCOMPARE(lines[0].toObject().value("tag").toString(), QString("tag_2"));
}
#ifndef Q_OS_WIN
void ProfileTest::testRemoveFavoriteThumb()
{
	Favorite fav("tag_1", 20, m_dates[0]);

	QDir(m_profile->getPath()).mkdir("thumb");
	QFile thumb(m_profile->getPath() + "/thumbs/" + fav.getName(true) + ".png");
	thumb.open(QFile::WriteOnly | QFile::Truncate);
	thumb.write(QString("test").toUtf8());
	thumb.close();

	QVERIFY(thumb.exists());
	m_profile->removeFavorite(fav);
	QVERIFY(!thumb.exists());
}
#endif


QTEST_MAIN(ProfileTest)
