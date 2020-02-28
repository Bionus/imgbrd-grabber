#include <QNetworkCookieJar>
#include <QSettings>
#include <QSignalSpy>
#include "auth/auth-const-field.h"
#include "auth/auth-field.h"
#include "auth/http-auth.h"
#include "custom-network-access-manager.h"
#include "login/http-get-login.h"
#include "login/http-post-login.h"
#include "mixed-settings.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "network/network-manager.h"
#include "catch.h"
#include "source-helpers.h"


template <class T>
void testLogin(const QString &type, const QString &url, Login::Result expected, Site *site, NetworkManager *manager)
{
	// Clear all cookies
	manager->setCookieJar(new QNetworkCookieJar(manager));

	QList<AuthField*> fields;
	HttpAuth auth(type, "/login", fields, "test_cookie");
	T login(&auth, site, manager, site->settings());

	REQUIRE(login.isTestable());

	CustomNetworkAccessManager::NextFiles.enqueue(url);

	QSignalSpy spy(&login, SIGNAL(loggedIn(Login::Result)));
	login.login();
	REQUIRE(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	auto result = arguments.at(0).value<Login::Result>();

	REQUIRE(result == expected);
}


TEST_CASE("HttpLogin")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	const QScopedPointer<Profile> profile(makeProfile());
	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	NetworkManager accessManager;

	SECTION("NonTestable")
	{
		QList<AuthField*> fields;
		HttpAuth auth("url", "", fields, "");
		HttpGetLogin login(&auth, site, &accessManager, site->settings());

		REQUIRE(!login.isTestable());
	}

	SECTION("LoginSuccess")
	{
		testLogin<HttpGetLogin>("get", "cookie", Login::Result::Success, site, &accessManager);
		testLogin<HttpPostLogin>("post", "cookie", Login::Result::Success, site, &accessManager);
	}

	SECTION("LoginFailure")
	{
		testLogin<HttpGetLogin>("get", "404", Login::Result::Failure, site, &accessManager);
		testLogin<HttpPostLogin>("post", "404", Login::Result::Failure, site, &accessManager);
	}

	SECTION("DoubleLogin")
	{
		testLogin<HttpGetLogin>("get", "cookie", Login::Result::Success, site, &accessManager);
		testLogin<HttpGetLogin>("get", "cookie", Login::Result::Success, site, &accessManager);

		testLogin<HttpPostLogin>("post", "cookie", Login::Result::Success, site, &accessManager);
		testLogin<HttpPostLogin>("post", "cookie", Login::Result::Success, site, &accessManager);
	}
}
