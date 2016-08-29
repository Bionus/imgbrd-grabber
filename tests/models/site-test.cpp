#include <QtTest>
#include "site-test.h"


void SiteTest::init()
{
	QDir().mkpath("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us");
	QFile::copy("release/sites/Danbooru (2.0)/model.xml", "tests/resources/sites/Danbooru (2.0)/model.xml");
	QFile::copy("release/sites/Danbooru (2.0)/sites.txt", "tests/resources/sites/Danbooru (2.0)/sites.txt");
	QFile::copy("release/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", "tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini");

	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);
	m_site = new Site(m_settings, "release/sites/Danbooru (2.0)", "danbooru.donmai.us");}

void SiteTest::cleanup()
{
	m_settings->deleteLater();
	m_site->deleteLater();
}


void SiteTest::testSetUsername()
{
	QString username = "test";

	QVERIFY(m_site->username() != username);
	m_site->setUsername(username);
	QVERIFY(m_site->username() == username);
}

void SiteTest::testSetPassword()
{
	QString password = "test";

	QVERIFY(m_site->password() != password);
	m_site->setPassword(password);
	QVERIFY(m_site->password() == password);
}

void SiteTest::testFixUrlBasic()
{
	QCOMPARE(m_site->fixUrl(""), QUrl());
	QCOMPARE(m_site->fixUrl("http://test.com/dir/toto.jpg"), QUrl("http://test.com/dir/toto.jpg"));
	QCOMPARE(m_site->fixUrl("//test.com/dir/toto.jpg"), QUrl("http://test.com/dir/toto.jpg"));
}
void SiteTest::testFixUrlRoot()
{
	QCOMPARE(m_site->fixUrl("/dir/toto.jpg"), QUrl("http://danbooru.donmai.us/dir/toto.jpg"));
	QCOMPARE(m_site->fixUrl("dir/toto.jpg"), QUrl("http://danbooru.donmai.us/dir/toto.jpg"));
}
void SiteTest::testFixUrlRelative()
{
	QCOMPARE(m_site->fixUrl("dir/toto.jpg", QUrl("http://test.com/dir/")), QUrl("http://test.com/dir/dir/toto.jpg"));
	QCOMPARE(m_site->fixUrl("toto.jpg", QUrl("http://test.com/dir/file.html")), QUrl("http://test.com/dir/toto.jpg"));
	QCOMPARE(m_site->fixUrl("toto.jpg", QUrl("http://test.com/dir/")), QUrl("http://test.com/dir/toto.jpg"));
}

void SiteTest::testGetSites()
{
	QList<Site*> sites;

	sites = Site::getSites(QStringList() << "danbooru.donmai.us");
	QCOMPARE(sites.count(), 1);
	QCOMPARE(sites.first()->url(), QString("danbooru.donmai.us"));
	QCOMPARE(sites.first()->type(), QString("Danbooru (2.0)"));

	sites = Site::getSites(QStringList() << "test (does not exist)" << "danbooru.donmai.us");
	QCOMPARE(sites.count(), 1);
	QCOMPARE(sites.first()->url(), QString("danbooru.donmai.us"));
	QCOMPARE(sites.first()->type(), QString("Danbooru (2.0)"));
}

void SiteTest::testGetAllCached()
{
	QMap<QString, Site*> *sites1 = Site::getAllSites();
	QMap<QString, Site*> *sites2 = Site::getAllSites();

	QCOMPARE(sites1, sites2);
}


static SiteTest instance;
