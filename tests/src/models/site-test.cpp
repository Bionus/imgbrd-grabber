#include <QNetworkCookie>
#include <QPointer>
#include <QSettings>
#include <QSignalSpy>
#include <QTimer>
#include "custom-network-access-manager.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "tags/tag.h"
#include "catch.h"
#include "source-helpers.h"


TEST_CASE("Site")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	auto profile = QPointer<Profile>(makeProfile());
	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	SECTION("DefaultApis")
	{
		QSettings settings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
		settings.setValue("sources/usedefault", false);
		settings.setValue("sources/source_1", "");
		settings.setValue("sources/source_2", "");
		settings.setValue("sources/source_3", "");
		settings.setValue("sources/source_4", "");

		Source source(profile, "tests/resources/sites/Danbooru (2.0)");
		Site site("danbooru.donmai.us", &source);

		REQUIRE(site.getApis().count() == 3);
	}

	SECTION("NoApis")
	{
		QSettings settings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
		settings.setValue("sources/usedefault", false);
		settings.setValue("sources/source_1", "1");
		settings.setValue("sources/source_2", "2");
		settings.setValue("sources/source_3", "3");
		settings.setValue("sources/source_4", "4");

		Source source(profile, "tests/resources/sites/Danbooru (2.0)");
		Site site("danbooru.donmai.us", &source);

		REQUIRE(site.getApis().count() == 0);
	}

	SECTION("FixUrlBasic")
	{
		REQUIRE(site->fixUrl("") == QUrl());
		REQUIRE(site->fixUrl("http://test.com/dir/toto.jpg") == QUrl("http://test.com/dir/toto.jpg"));
		REQUIRE(site->fixUrl("http://danbooru.donmai.us/dir/toto.jpg") == QUrl("https://danbooru.donmai.us/dir/toto.jpg"));
		REQUIRE(site->fixUrl("//test.com/dir/toto.jpg") == QUrl("https://test.com/dir/toto.jpg"));
	}
	SECTION("FixUrlRoot")
	{
		REQUIRE(site->fixUrl("/dir/toto.jpg") == QUrl("https://danbooru.donmai.us/dir/toto.jpg"));
		REQUIRE(site->fixUrl("dir/toto.jpg") == QUrl("https://danbooru.donmai.us/dir/toto.jpg"));
	}
	SECTION("FixUrlRelative")
	{
		REQUIRE(site->fixUrl("dir/toto.jpg", QUrl("http://test.com/dir/")) == QUrl("http://test.com/dir/dir/toto.jpg"));
		REQUIRE(site->fixUrl("toto.jpg", QUrl("http://test.com/dir/file.html")) == QUrl("http://test.com/dir/toto.jpg"));
		REQUIRE(site->fixUrl("toto.jpg", QUrl("http://test.com/dir/")) == QUrl("http://test.com/dir/toto.jpg"));
	}

	SECTION("GetSites")
	{
		QList<Site*> sites;

		sites = profile->getFilteredSites(QStringList() << "danbooru.donmai.us");
		REQUIRE(sites.count() == 1);
		REQUIRE(sites.first()->url() == QString("danbooru.donmai.us"));
		REQUIRE(sites.first()->type() == QString("Danbooru (2.0)"));

		sites = profile->getFilteredSites(QStringList() << "test (does not exist)" << "danbooru.donmai.us");
		REQUIRE(sites.count() == 1);
		REQUIRE(sites.first()->url() == QString("danbooru.donmai.us"));
		REQUIRE(sites.first()->type() == QString("Danbooru (2.0)"));
	}

	SECTION("Cookies")
	{
		QList<QNetworkCookie> cookies;
		cookies.append(QNetworkCookie("test_name_1", "test_value_1"));
		cookies.append(QNetworkCookie("test_name_2", "test_value_2"));

		QList<QVariant> cookiesVariant;
		cookiesVariant.reserve(cookies.count());
		for (const QNetworkCookie &cookie : cookies) {
			cookiesVariant.append(cookie.toRawForm());
		}
		QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
		siteSettings.setValue("cookies", cookiesVariant);
		siteSettings.sync();

		site->loadConfig();
		QList<QNetworkCookie> siteCookies(site->cookies());

		REQUIRE(siteCookies.count() == cookies.count());
		REQUIRE(siteCookies[0].name() == cookies[0].name());
		REQUIRE(siteCookies[0].value() == cookies[0].value());
		REQUIRE(siteCookies[1].name() == cookies[1].name());
		REQUIRE(siteCookies[1].value() == cookies[1].value());
	}

	SECTION("LoginNone")
	{
		// Prepare settings
		QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
		siteSettings.setValue("login/parameter", true);
		site->loadConfig();

		// Wait for login
		QSignalSpy spy(site, SIGNAL(loggedIn(Site*, Site::LoginResult)));
		QTimer::singleShot(0, site, SLOT(login()));
		REQUIRE(spy.wait());

		// Get result
		QList<QVariant> arguments = spy.takeFirst();
		Site::LoginResult result = arguments.at(1).value<Site::LoginResult>();

		REQUIRE(result == Site::LoginResult::Impossible);
	}

	SECTION("LoginGet")
	{
		// FIXME: with the new auth system, you can't override auth information like this
		return;

		// Prepare settings
		QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini", QSettings::IniFormat);
		siteSettings.setValue("auth/pseudo", "user");
		siteSettings.setValue("auth/password", "somepassword");
		siteSettings.setValue("login/type", "get");
		siteSettings.setValue("login/get/pseudo", "name");
		siteSettings.setValue("login/get/password", "password");
		siteSettings.setValue("login/get/url", "/session/new");
		siteSettings.setValue("login/get/cookie", "_danbooru_session");
		site->loadConfig();

		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/login.html");

		// Wait for login
		QSignalSpy spy(site, SIGNAL(loggedIn(Site*, Site::LoginResult)));
		QTimer *timer = new QTimer();
		timer->setSingleShot(true);
		QObject::connect(timer, &QTimer::timeout, [=]() {
			site->login(true);
			timer->deleteLater();
		});
		timer->start(0);
		REQUIRE(spy.wait());

		// Get result
		QList<QVariant> arguments = spy.takeFirst();
		Site::LoginResult result = arguments.at(1).value<Site::LoginResult>();

		REQUIRE(result == Site::LoginResult::Error);
	}

	SECTION("LoginPost")
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
		site->loadConfig();

		// Wait for login
		QSignalSpy spy(site, SIGNAL(loggedIn(Site*, Site::LoginResult)));
		QTimer *timer = new QTimer();
		timer->setSingleShot(true);
		QObject::connect(timer, &QTimer::timeout, [=]() {
			site->login(true);
			timer->deleteLater();
		});
		timer->start(0);
		REQUIRE(spy.wait());

		// Get result
		QList<QVariant> arguments = spy.takeFirst();
		Site::LoginResult result = arguments.at(1).value<Site::LoginResult>();

		REQUIRE(result == Site::LoginResult::Error);
	}
}
