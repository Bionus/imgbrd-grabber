#include <QSettings>
#include <QSignalSpy>
#include "auth/auth-const-field.h"
#include "auth/auth-field.h"
#include "auth/url-auth.h"
#include "custom-network-access-manager.h"
#include "login/url-login.h"
#include "mixed-settings.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "network/network-manager.h"
#include "catch.h"
#include "source-helpers.h"


TEST_CASE("UrlLogin")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	auto profile = QPointer<Profile>(makeProfile());
	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	NetworkManager accessManager;

	SECTION("NonTestable")
	{
		QList<AuthField*> fields;
		UrlAuth auth("url", fields, 0);
		UrlLogin login(&auth, site, &accessManager, site->settings());

		REQUIRE(!login.isTestable());
	}

	SECTION("LoginSuccess")
	{
		MixedSettings *settings = site->settings();
		settings->setValue("login/type", "disabled");
		site->loadConfig();

		QList<AuthField*> fields;
		UrlAuth auth("url", fields, 10);
		UrlLogin login(&auth, site, &accessManager, settings);

		REQUIRE(login.isTestable());

		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/results.xml");

		QSignalSpy spy(&login, SIGNAL(loggedIn(Login::Result)));
		login.login();
		REQUIRE(spy.wait());

		QList<QVariant> arguments = spy.takeFirst();
		auto result = arguments.at(0).value<Login::Result>();

		REQUIRE(result == Login::Success);
	}

	SECTION("LoginFailure")
	{
		MixedSettings *settings = site->settings();
		settings->setValue("login/type", "disabled");
		site->loadConfig();

		QList<AuthField*> fields;
		UrlAuth auth("url", fields, 10);
		UrlLogin login(&auth, site, &accessManager, settings);

		REQUIRE(login.isTestable());

		for (int i = 0; i < 3; ++i) {
			CustomNetworkAccessManager::NextFiles.enqueue("404");
		}

		QSignalSpy spy(&login, SIGNAL(loggedIn(Login::Result)));
		login.login();
		REQUIRE(spy.wait());

		QList<QVariant> arguments = spy.takeFirst();
		auto result = arguments.at(0).value<Login::Result>();

		REQUIRE(result == Login::Failure);
	}

	SECTION("ComplementUrl")
	{
		QList<AuthField*> fields;
		fields.append(new AuthConstField("a", "1"));
		fields.append(new AuthConstField("b", ""));
		fields.append(new AuthConstField("c", "2"));

		UrlAuth auth("url", fields, 10);
		UrlLogin login(&auth, site, &accessManager, site->settings());

		REQUIRE(login.complementUrl("/") == QString("/?a=1&c=2"));
		REQUIRE(login.complementUrl("/?ho=&test=1") == QString("/?ho=&test=1&a=1&c=2"));
	}
}
