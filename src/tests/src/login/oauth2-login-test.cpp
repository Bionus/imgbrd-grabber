#include <QNetworkRequest>
#include <QSettings>
#include <QSignalSpy>
#include "auth/oauth2-auth.h"
#include "custom-network-access-manager.h"
#include "login/oauth2-login.h"
#include "mixed-settings.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "network/network-manager.h"
#include "catch.h"
#include "source-helpers.h"


void testLogin(const QString &type, const QString &url, Login::Result expected, const QString &expectedHeader, Site *site, NetworkManager *manager)
{
	MixedSettings *settings = site->settings();
	settings->setValue("auth/consumerKey", "consumerKey");
	settings->setValue("auth/consumerSecret", "consumerSecret");
	settings->setValue("auth/accessToken", "");
	settings->setValue("auth/refreshToken", "");

	OAuth2Auth auth("oauth2", type, "/token");
	OAuth2Login login(&auth, site, manager, settings);

	REQUIRE(login.isTestable());

	CustomNetworkAccessManager::NextFiles.enqueue(url);

	QSignalSpy spy(&login, SIGNAL(loggedIn(Login::Result)));
	login.login();
	REQUIRE(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	Login::Result result = arguments.at(0).value<Login::Result>();

	REQUIRE(result == expected);

	if (!expectedHeader.isEmpty()) {
		QNetworkRequest req;
		login.complementRequest(&req);

		REQUIRE(QString(req.rawHeader("Authorization")) == expectedHeader);
	}
}

TEST_CASE("OAuth2Login")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	const QScopedPointer<Profile> profile(makeProfile());
	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	NetworkManager accessManager;

	SECTION("NonTestable")
	{
		OAuth2Auth auth("oauth2", "password", "");
		OAuth2Login login(&auth, site, &accessManager, site->settings());

		REQUIRE(!login.isTestable());
	}

	SECTION("LoginSuccess")
	{
		testLogin("header_basic", "tests/resources/oauth2/ok.json", Login::Result::Success, "Bearer test_token", site, &accessManager);
		testLogin("client_credentials", "tests/resources/oauth2/ok_in_response.json", Login::Result::Success, "Bearer test_token", site, &accessManager);
		testLogin("password", "tests/resources/oauth2/ok.json", Login::Result::Success, "Bearer test_token", site, &accessManager);
	}

	SECTION("LoginFailure")
	{
		testLogin("header_basic", "404", Login::Result::Failure, QString(), site, &accessManager);
		testLogin("client_credentials", "tests/resources/oauth2/no_token_type.json", Login::Result::Failure, QString(), site, &accessManager);
		testLogin("password", "tests/resources/oauth2/wrong_token_type.json", Login::Result::Failure, QString(), site, &accessManager);
	}
}
