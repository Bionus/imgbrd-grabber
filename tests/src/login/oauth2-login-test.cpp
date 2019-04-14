#include "login/oauth2-login-test.h"
#include <QSettings>
#include <QtTest>
#include "auth/oauth2-auth.h"
#include "custom-network-access-manager.h"
#include "login/oauth2-login.h"
#include "mixed-settings.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


void OAuth2LoginTest::init()
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	m_profile = new Profile("tests/resources/");
	m_site = m_profile->getSites().value("danbooru.donmai.us");
}

void OAuth2LoginTest::cleanup()
{
	m_profile->deleteLater();
}


void OAuth2LoginTest::testNonTestable()
{
	OAuth2Auth auth("oauth2", "password", "");
	OAuth2Login login(&auth, m_site, &m_manager, m_site->settings());

	QVERIFY(!login.isTestable());
}

void testLogin(const QString &type, const QString &url, Login::Result expected, const QString &expectedHeader, Site *site, NetworkManager *manager)
{
	MixedSettings *settings = site->settings();
	settings->setValue("auth/consumerKey", "consumerKey");
	settings->setValue("auth/consumerSecret", "consumerSecret");
	settings->setValue("auth/accessToken", "");
	settings->setValue("auth/refreshToken", "");

	OAuth2Auth auth("oauth2", type, "/token");
	OAuth2Login login(&auth, site, manager, settings);

	QVERIFY(login.isTestable());

	CustomNetworkAccessManager::NextFiles.enqueue(url);

	QSignalSpy spy(&login, SIGNAL(loggedIn(Login::Result)));
	login.login();
	QVERIFY(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	Login::Result result = arguments.at(0).value<Login::Result>();

	QCOMPARE(result, expected);

	if (!expectedHeader.isEmpty()) {
		QNetworkRequest req;
		login.complementRequest(&req);

		QCOMPARE(QString(req.rawHeader("Authorization")), expectedHeader);
	}
}

void OAuth2LoginTest::testLoginSuccess()
{
	testLogin("header_basic", "tests/resources/oauth2/ok.json", Login::Result::Success, "Bearer test_token", m_site, &m_manager);
	testLogin("client_credentials", "tests/resources/oauth2/ok_in_response.json", Login::Result::Success, "Bearer test_token", m_site, &m_manager);
	testLogin("password", "tests/resources/oauth2/ok.json", Login::Result::Success, "Bearer test_token", m_site, &m_manager);
}

void OAuth2LoginTest::testLoginFailure()
{
	testLogin("header_basic", "404", Login::Result::Failure, QString(), m_site, &m_manager);
	testLogin("client_credentials", "tests/resources/oauth2/no_token_type.json", Login::Result::Failure, QString(), m_site, &m_manager);
	testLogin("password", "tests/resources/oauth2/wrong_token_type.json", Login::Result::Failure, QString(), m_site, &m_manager);
}


QTEST_MAIN(OAuth2LoginTest)
