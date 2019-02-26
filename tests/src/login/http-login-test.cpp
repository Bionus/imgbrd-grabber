#include "login/http-login-test.h"
#include <QNetworkCookieJar>
#include <QSettings>
#include <QtTest>
#include "auth/auth-const-field.h"
#include "auth/auth-field.h"
#include "auth/http-auth.h"
#include "login/http-get-login.h"
#include "login/http-post-login.h"
#include "mixed-settings.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


void HttpLoginTest::init()
{
	m_profile = new Profile("tests/resources/settings.ini");
	m_source = new Source(m_profile, "release/sites/Danbooru (2.0)");
	m_site = new Site("danbooru.donmai.us", m_source);
}

void HttpLoginTest::cleanup()
{
	m_profile->deleteLater();
	m_source->deleteLater();
	m_site->deleteLater();
}


void HttpLoginTest::testNonTestable()
{
	QList<AuthField*> fields;
	HttpAuth auth("url", "", fields, "");
	HttpGetLogin login(&auth, m_site, &m_manager, m_site->settings());

	QVERIFY(!login.isTestable());
}

template <class T>
void testLogin(const QString &type, const QString &url, Login::Result expected, Site *site, CustomNetworkAccessManager *manager)
{
	// Clear all cookies
	manager->setCookieJar(new QNetworkCookieJar(manager));

	QList<AuthField*> fields;
	HttpAuth auth(type, "/login", fields, "test_cookie");
	T login(&auth, site, manager, site->settings());

	QVERIFY(login.isTestable());

	CustomNetworkAccessManager::NextFiles.enqueue(url);

	QSignalSpy spy(&login, SIGNAL(loggedIn(Login::Result)));
	login.login();
	QVERIFY(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	Login::Result result = arguments.at(0).value<Login::Result>();

	QCOMPARE(result, expected);
}

void HttpLoginTest::testLoginSuccess()
{
	testLogin<HttpGetLogin>("get", "cookie", Login::Result::Success, m_site, &m_manager);
	testLogin<HttpPostLogin>("post", "cookie", Login::Result::Success, m_site, &m_manager);
}

void HttpLoginTest::testLoginFailure()
{
	testLogin<HttpGetLogin>("get", "404", Login::Result::Failure, m_site, &m_manager);
	testLogin<HttpPostLogin>("post", "404", Login::Result::Failure, m_site, &m_manager);
}

void HttpLoginTest::testDoubleLogin()
{
	testLogin<HttpGetLogin>("get", "cookie", Login::Result::Success, m_site, &m_manager);
	testLogin<HttpGetLogin>("get", "cookie", Login::Result::Success, m_site, &m_manager);

	testLogin<HttpPostLogin>("post", "cookie", Login::Result::Success, m_site, &m_manager);
	testLogin<HttpPostLogin>("post", "cookie", Login::Result::Success, m_site, &m_manager);
}


QTEST_MAIN(HttpLoginTest)
