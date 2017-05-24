#include <QtTest>
#include <QNetworkCookie>
#include "site-test.h"


void SiteTest::init()
{
	QDir().mkpath("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us");
	QFile::remove("tests/resources/sites/Danbooru (2.0)/model.xml");
	QFile::remove("tests/resources/sites/Danbooru (2.0)/sites.txt");
	QFile::remove("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini");
	QFile::copy("release/sites/Danbooru (2.0)/model.xml", "tests/resources/sites/Danbooru (2.0)/model.xml");
	QFile::copy("release/sites/Danbooru (2.0)/sites.txt", "tests/resources/sites/Danbooru (2.0)/sites.txt");
	QFile::copy("release/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", "tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini");

	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);
	m_source = new Source(&profile, "tests/resources/sites/Danbooru (2.0)");
	m_site = new Site("danbooru.donmai.us", m_source);
}

void SiteTest::cleanup()
{
	m_settings->deleteLater();
	m_source->deleteLater();
	m_site->deleteLater();
}


void SiteTest::testDefaultApis()
{
	QSettings settings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", QSettings::IniFormat);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", "");
	settings.setValue("sources/source_2", "");
	settings.setValue("sources/source_3", "");
	settings.setValue("sources/source_4", "");

	Source source(&profile, "tests/resources/sites/Danbooru (2.0)");
	Site site("danbooru.donmai.us", &source);

	QCOMPARE(site.getApis().count(), 3);
}

void SiteTest::testNoApis()
{
	QSettings settings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", QSettings::IniFormat);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", "1");
	settings.setValue("sources/source_2", "2");
	settings.setValue("sources/source_3", "3");
	settings.setValue("sources/source_4", "4");

	Source source(&profile, "tests/resources/sites/Danbooru (2.0)");
	Site site("danbooru.donmai.us", &source);

	QCOMPARE(site.getApis().count(), 0);
	QCOMPARE(site.contains("Urls/Image"), false);
	QCOMPARE(site.value("Urls/Image"), QString());
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
	QCOMPARE(m_site->fixUrl("http://test.com/dir/toto.jpg"), QUrl("https://test.com/dir/toto.jpg"));
	QCOMPARE(m_site->fixUrl("//test.com/dir/toto.jpg"), QUrl("https://test.com/dir/toto.jpg"));
}
void SiteTest::testFixUrlRoot()
{
	QCOMPARE(m_site->fixUrl("/dir/toto.jpg"), QUrl("https://danbooru.donmai.us/dir/toto.jpg"));
	QCOMPARE(m_site->fixUrl("dir/toto.jpg"), QUrl("https://danbooru.donmai.us/dir/toto.jpg"));
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

	sites = Site::getSites(&profile, QStringList() << "danbooru.donmai.us");
	QCOMPARE(sites.count(), 1);
	QCOMPARE(sites.first()->url(), QString("danbooru.donmai.us"));
	QCOMPARE(sites.first()->type(), QString("Danbooru (2.0)"));

	sites = Site::getSites(&profile, QStringList() << "test (does not exist)" << "danbooru.donmai.us");
	QCOMPARE(sites.count(), 1);
	QCOMPARE(sites.first()->url(), QString("danbooru.donmai.us"));
	QCOMPARE(sites.first()->type(), QString("Danbooru (2.0)"));
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
	siteSettings.sync();

	m_site->loadConfig();
	QList<QNetworkCookie> siteCookies(m_site->cookies());

	QCOMPARE(siteCookies.count(), cookies.count());
	QCOMPARE(siteCookies[0].name(), cookies[0].name());
	QCOMPARE(siteCookies[0].value(), cookies[0].value());
	QCOMPARE(siteCookies[1].name(), cookies[1].name());
	QCOMPARE(siteCookies[1].value(), cookies[1].value());
}

void SiteTest::testLoginNone()
{
	// Prepare settings
	QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", QSettings::IniFormat);
	siteSettings.setValue("login/parameter", true);
	m_site->loadConfig();

	// Wait for login
	QSignalSpy spy(m_site, SIGNAL(loggedIn(Site*, Site::LoginResult)));
	QTimer::singleShot(0, m_site, SLOT(login()));
	QVERIFY(spy.wait());

	// Get result
	QList<QVariant> arguments = spy.takeFirst();
	Site::LoginResult result = arguments.at(1).value<Site::LoginResult>();

	QCOMPARE(result, Site::LoginResult::Impossible);
}

void SiteTest::testLoginGet()
{
	// Prepare settings
	QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", QSettings::IniFormat);
	siteSettings.setValue("auth/pseudo", "user");
	siteSettings.setValue("auth/password", "somepassword");
	siteSettings.setValue("login/parameter", false);
	siteSettings.setValue("login/method", "get");
	siteSettings.setValue("login/pseudo", "name");
	siteSettings.setValue("login/password", "password");
	siteSettings.setValue("login/url", "/session/new");
	siteSettings.setValue("login/cookie", "_danbooru_session");
	m_site->loadConfig();

	// Wait for login
	QSignalSpy spy(m_site, SIGNAL(loggedIn(Site*, Site::LoginResult)));
	QTimer *timer = new QTimer(this);
	timer->setSingleShot(true);
	connect(timer, &QTimer::timeout, [=]() {
		m_site->login(true);
		timer->deleteLater();
	});
	timer->start(0);
	QVERIFY(spy.wait());

	// Get result
	QList<QVariant> arguments = spy.takeFirst();
	Site::LoginResult result = arguments.at(1).value<Site::LoginResult>();

	QCOMPARE(result, Site::LoginResult::Error);
}

void SiteTest::testLoginPost()
{
	// Prepare settings
	QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", QSettings::IniFormat);
	siteSettings.setValue("auth/pseudo", "user");
	siteSettings.setValue("auth/password", "somepassword");
	siteSettings.setValue("login/parameter", false);
	siteSettings.setValue("login/method", "post");
	siteSettings.setValue("login/pseudo", "name");
	siteSettings.setValue("login/password", "password");
	siteSettings.setValue("login/url", "/session");
	siteSettings.setValue("login/cookie", "_danbooru_session");
	m_site->loadConfig();

	// Wait for login
	QSignalSpy spy(m_site, SIGNAL(loggedIn(Site*, Site::LoginResult)));
	QTimer *timer = new QTimer(this);
	timer->setSingleShot(true);
	connect(timer, &QTimer::timeout, [=]() {
		m_site->login(true);
		timer->deleteLater();
	});
	timer->start(0);
	QVERIFY(spy.wait());

	// Get result
	QList<QVariant> arguments = spy.takeFirst();
	Site::LoginResult result = arguments.at(1).value<Site::LoginResult>();

	QCOMPARE(result, Site::LoginResult::Error);
}


static SiteTest instance;
