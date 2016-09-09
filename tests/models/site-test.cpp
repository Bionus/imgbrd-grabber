#include <QtTest>
#include <QNetworkCookie>
#include "site-test.h"


void SiteTest::init()
{
	QDir().mkpath("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us");
	QFile::copy("release/sites/Danbooru (2.0)/model.xml", "tests/resources/sites/Danbooru (2.0)/model.xml");
	QFile::copy("release/sites/Danbooru (2.0)/sites.txt", "tests/resources/sites/Danbooru (2.0)/sites.txt");
	QFile::copy("release/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", "tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini");

	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);
	m_site = new Site(m_settings, "tests/resources/sites/Danbooru (2.0)", "danbooru.donmai.us");}

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

void SiteTest::testLoadTags()
{
	// Wait for tags
	QSignalSpy spy(m_site, SIGNAL(finishedLoadingTags(QList<Tag>)));
	m_site->loadTags(3, 20);
	QVERIFY(spy.wait());

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	QVariantList variants = arguments.at(0).value<QVariantList>();

	// Convert results
	QList<Tag> tags;
	QStringList tagsText;
	for (QVariant variant : variants)
	{
		Tag tag = variant.value<Tag>();
		tags.append(tag);
		tagsText.append(tag.text());
	}

	// Compare results
	tagsText = tagsText.mid(0, 3);
	QCOMPARE(tags.count(), 20);
	QCOMPARE(tagsText, QStringList() << "kameji_(tyariri)" << "the_king_of_fighterx_xiv" << "condom_skirt");
}

void SiteTest::testCheckForUpdates()
{
	// Wait for tags
	QSignalSpy spy(m_site, SIGNAL(checkForUpdatesFinished(Site*)));
	m_site->checkForUpdates();
	QVERIFY(spy.wait());

	// Check result
	QVERIFY(!m_site->updateVersion().isEmpty());
}

void SiteTest::testCookies()
{
	QList<QNetworkCookie> cookies;
	cookies.append(QNetworkCookie("test_name_1", "test_value_1"));
	cookies.append(QNetworkCookie("test_name_2", "test_value_2"));

	QList<QVariant> cookiesVariant;
	for (QNetworkCookie cookie : cookies)
	{
		cookiesVariant.append(cookie.toRawForm());
	}
	QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", QSettings::IniFormat);
	siteSettings.setValue("cookies", cookiesVariant);

	m_site->load("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini");
	QList<QNetworkCookie> siteCookies(m_site->cookies());

	QCOMPARE(siteCookies.count(), cookies.count());
	QCOMPARE(siteCookies[0].name(), cookies[0].name());
	QCOMPARE(siteCookies[0].value(), cookies[0].value());
	QCOMPARE(siteCookies[1].name(), cookies[1].name());
	QCOMPARE(siteCookies[1].value(), cookies[1].value());
}


static SiteTest instance;
