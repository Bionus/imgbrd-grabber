#include "site-test.h"
#include <QNetworkCookie>
#include <QtTest>


void SiteTest::init()
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	m_profile = new Profile("tests/resources/");
	m_settings = m_profile->getSettings();
	m_source = new Source(m_profile, "tests/resources/sites/Danbooru (2.0)");
	m_site = new Site("danbooru.donmai.us", m_source);
}

void SiteTest::cleanup()
{
	m_profile->deleteLater();
	m_source->deleteLater();
	m_site->deleteLater();
}


void SiteTest::testDefaultApis()
{
	QSettings settings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", "");
	settings.setValue("sources/source_2", "");
	settings.setValue("sources/source_3", "");
	settings.setValue("sources/source_4", "");

	Source source(m_profile, "tests/resources/sites/Danbooru (2.0)");
	Site site("danbooru.donmai.us", &source);

	QCOMPARE(site.getApis().count(), 3);
}

void SiteTest::testNoApis()
{
	QSettings settings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", "1");
	settings.setValue("sources/source_2", "2");
	settings.setValue("sources/source_3", "3");
	settings.setValue("sources/source_4", "4");

	Source source(m_profile, "tests/resources/sites/Danbooru (2.0)");
	Site site("danbooru.donmai.us", &source);

	QCOMPARE(site.getApis().count(), 0);
	QCOMPARE(site.contains("Urls/Image"), false);
	QCOMPARE(site.value("Urls/Image"), QString());
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

	sites = m_profile->getFilteredSites(QStringList() << "danbooru.donmai.us");
	QCOMPARE(sites.count(), 1);
	QCOMPARE(sites.first()->url(), QString("danbooru.donmai.us"));
	QCOMPARE(sites.first()->type(), QString("Danbooru (2.0)"));

	sites = m_profile->getFilteredSites(QStringList() << "test (does not exist)" << "danbooru.donmai.us");
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
	QVector<Tag> tags;
	QStringList tagsText;
	tags.reserve(variants.count());
	tagsText.reserve(variants.count());
	for (const QVariant &variant : variants)
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
	cookiesVariant.reserve(cookies.count());
	for (const QNetworkCookie &cookie : cookies)
	{
		cookiesVariant.append(cookie.toRawForm());
	}
	QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
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
	QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
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
	QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
	siteSettings.setValue("auth/pseudo", "user");
	siteSettings.setValue("auth/password", "somepassword");
	siteSettings.setValue("login/type", "get");
	siteSettings.setValue("login/get/pseudo", "name");
	siteSettings.setValue("login/get/password", "password");
	siteSettings.setValue("login/get/url", "/session/new");
	siteSettings.setValue("login/get/cookie", "_danbooru_session");
	m_site->loadConfig();

	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/login.html");

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
	QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
	siteSettings.setValue("auth/pseudo", "user");
	siteSettings.setValue("auth/password", "somepassword");
	siteSettings.setValue("login/type", "post");
	siteSettings.setValue("login/post/pseudo", "name");
	siteSettings.setValue("login/post/password", "password");
	siteSettings.setValue("login/post/url", "/session");
	siteSettings.setValue("login/post/cookie", "_danbooru_session");
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
