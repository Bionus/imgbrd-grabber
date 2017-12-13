#include "profile-test.h"


void ProfileTest::init()
{
	m_dates.clear();
	m_dates.append(QDateTime(QDate(2016, 9, 1), QTime(9, 23, 17)));
	m_dates.append(QDateTime(QDate(2016, 10, 1), QTime(12, 23, 17)));
	m_dates.append(QDateTime(QDate(2016, 7, 1), QTime(9, 12, 17)));

	QFile f("tests/resources/favorites.txt");
	f.open(QFile::WriteOnly | QFile::Text);
	f.write(Favorite("tag_1", 20, m_dates[0], 60, m_dates[0]).toString().toUtf8() + "\r\n");
	f.write(Favorite("tag_2", 100, m_dates[1], 360, m_dates[1]).toString().toUtf8() + "\r\n");
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
	Favorite fav("tag_3", 70, m_dates[2], 120, m_dates[2]);
	m_profile->addFavorite(fav);
	m_profile->sync();

	QFile f("tests/resources/favorites.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 3);
	QCOMPARE(lines[0], Favorite("tag_1", 20, m_dates[0], 60, m_dates[0]).toString());
	QCOMPARE(lines[1], Favorite("tag_2", 100, m_dates[1], 360, m_dates[1]).toString());
	QCOMPARE(lines[2], fav.toString());
}

void ProfileTest::testRemoveFavorite()
{
	m_profile->removeFavorite(Favorite("tag_1", 20, m_dates[0], 60, m_dates[0]));
	m_profile->sync();

	QFile f("tests/resources/favorites.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 1);
	QCOMPARE(lines[0], Favorite("tag_2", 100, m_dates[1], 360, m_dates[1]).toString());
}
#ifndef Q_OS_WIN
void ProfileTest::testRemoveFavoriteThumb()
{
	Favorite fav("tag_1", 20, m_dates[0], 60, m_dates[0]);

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

void ProfileTest::testLoadMd5s()
{
	QCOMPARE(m_profile->md5Exists("5a105e8b9d40e1329780d62ea2265d8a"), QString("tests/resources/image_1x1.png"));
	QCOMPARE(m_profile->md5Exists("ad0234829205b9033196ba818f7a872b"), QString("tests/resources/image_1x1.png"));
}

void ProfileTest::testAddMd5()
{
	m_profile->addMd5("8ad8757baa8564dc136c1e07507f4a98", "tests/resources/image_1x1.png");
	QCOMPARE(m_profile->md5Exists("8ad8757baa8564dc136c1e07507f4a98"), QString("tests/resources/image_1x1.png"));

	m_profile->sync();

	QFile f("tests/resources/md5s.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 3);
	QVERIFY(lines.contains("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png"));
	QVERIFY(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
	QVERIFY(lines.contains("8ad8757baa8564dc136c1e07507f4a98tests/resources/image_1x1.png"));
}

void ProfileTest::testUpdateMd5()
{
	m_profile->setMd5("5a105e8b9d40e1329780d62ea2265d8a", "newpath.png");
	m_profile->sync();

	QFile f("tests/resources/md5s.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 2);
	QVERIFY(lines.contains("5a105e8b9d40e1329780d62ea2265d8anewpath.png"));
	QVERIFY(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
}

void ProfileTest::testRemoveMd5()
{
	m_profile->removeMd5("5a105e8b9d40e1329780d62ea2265d8a");
	QVERIFY(m_profile->md5Exists("5a105e8b9d40e1329780d62ea2265d8a").isEmpty());

	m_profile->sync();

	QFile f("tests/resources/md5s.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 1);
	QVERIFY(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
}


void ProfileTest::testMd5ActionDontKeepDeleted()
{
	m_profile->getSettings()->setValue("Save/md5Duplicates", "move");
	m_profile->getSettings()->setValue("Save/keepDeletedMd5", false);

	QPair<QString, QString> action;

	action = m_profile->md5Action("new");
	QCOMPARE(action.first, QString("move"));
	QCOMPARE(action.second, QString(""));

	m_profile->addMd5("new", "tests/resources/image_1x1.png");

	action = m_profile->md5Action("new");
	QCOMPARE(action.first, QString("move"));
	QCOMPARE(action.second, QString("tests/resources/image_1x1.png"));

	m_profile->removeMd5("new");

	action = m_profile->md5Action("new");
	QCOMPARE(action.first, QString("move"));
	QCOMPARE(action.second, QString(""));

	// Restore state
	m_profile->getSettings()->setValue("Save/md5Duplicates", "save");
}

void ProfileTest::testMd5ActionKeepDeleted()
{
	m_profile->getSettings()->setValue("Save/md5Duplicates", "move");
	m_profile->getSettings()->setValue("Save/keepDeletedMd5", true);

	QPair<QString, QString> action;

	action = m_profile->md5Action("new");
	QCOMPARE(action.first, QString("move"));
	QCOMPARE(action.second, QString(""));

	m_profile->addMd5("new", "NON_EXISTING_FILE");

	action = m_profile->md5Action("new");
	QCOMPARE(action.first, QString("ignore"));
	QCOMPARE(action.second, QString("NON_EXISTING_FILE"));

	// Restore state
	m_profile->removeMd5("new");
	m_profile->getSettings()->setValue("Save/md5Duplicates", "save");
	m_profile->getSettings()->setValue("Save/keepDeletedMd5", false);
}



static ProfileTest instance;
