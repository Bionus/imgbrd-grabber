#include "profile-test.h"


void ProfileTest::init()
{
	QFile f("tests/resources/favorites.txt");
	f.open(QFile::WriteOnly | QFile::Text);
	f.write(Favorite("tag_1", 20, QDateTime(QDate(2016, 9, 1), QTime(9, 23, 17))).toString().toUtf8() + "\r\n");
	f.write(Favorite("tag_2", 100, QDateTime(QDate(2016, 10, 1), QTime(12, 23, 17))).toString().toUtf8() + "\r\n");
	f.close();

	m_profile = new Profile("tests/resources/");
}

void ProfileTest::cleanup()
{
	delete m_profile;
}


void ProfileTest::testConstructorEmpty()
{
	Profile profile;

	QCOMPARE(profile.getPath(), QString());
	QVERIFY(profile.getSettings() == nullptr);
	QVERIFY(profile.getFavorites().isEmpty());
	QVERIFY(profile.getKeptForLater().isEmpty());
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
	QCOMPARE(favs[0].getLastViewed(), QDateTime(QDate(2016, 9, 1), QTime(9, 23, 17)));
	QCOMPARE(favs[1].getName(), QString("tag_2"));
	QCOMPARE(favs[1].getNote(), 100);
	QCOMPARE(favs[1].getLastViewed(), QDateTime(QDate(2016, 10, 1), QTime(12, 23, 17)));
}

void ProfileTest::testAddFavorite()
{
	Favorite fav("tag_3", 70, QDateTime(QDate(2016, 7, 1), QTime(9, 12, 17)));
	m_profile->addFavorite(fav);
	m_profile->sync();

	QFile f("tests/resources/favorites.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 3);
	QCOMPARE(lines[0], Favorite("tag_1", 20, QDateTime(QDate(2016, 9, 1), QTime(9, 23, 17))).toString());
	QCOMPARE(lines[1], Favorite("tag_2", 100, QDateTime(QDate(2016, 10, 1), QTime(12, 23, 17))).toString());
	QCOMPARE(lines[2], fav.toString());;
}

void ProfileTest::testRemoveFavorite()
{
	m_profile->removeFavorite(Favorite("tag_1", 20, QDateTime(QDate(2016, 9, 1), QTime(9, 23, 17))));
	m_profile->sync();

	QFile f("tests/resources/favorites.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 1);
	QCOMPARE(lines[0], Favorite("tag_2", 100, QDateTime(QDate(2016, 10, 1), QTime(12, 23, 17))).toString());
}


static ProfileTest instance;
