#include "login/url-login-test.h"
#include <QSettings>
#include <QtTest>
#include "auth/auth-field.h"
#include "auth/auth-const-field.h"
#include "auth/url-auth.h"
#include "login/url-login.h"
#include "mixed-settings.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


void UrlLoginTest::init()
{
	m_profile = new Profile("tests/resources/settings.ini");
	m_source = new Source(m_profile, "release/sites/Danbooru (2.0)");
	m_site = new Site("danbooru.donmai.us", m_source);
}

void UrlLoginTest::cleanup()
{
	MixedSettings *settings = m_site->settings();
	settings->setValue("login/type", "url");
	settings->sync();

	m_profile->deleteLater();
	m_source->deleteLater();
	m_site->deleteLater();
}


void UrlLoginTest::testNonTestable()
{
	QList<AuthField*> fields;
	UrlAuth auth("url", fields, 0);
	UrlLogin login(&auth, m_site, &m_manager, m_site->settings());

	QVERIFY(!login.isTestable());
}

void UrlLoginTest::testLoginSuccess()
{
	MixedSettings *settings = m_site->settings();
	settings->setValue("login/type", "disabled");
	m_site->loadConfig();

	QList<AuthField*> fields;
	UrlAuth auth("url", fields, 10);
	UrlLogin login(&auth, m_site, &m_manager, settings);

	QVERIFY(login.isTestable());

	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/results.xml");

	QSignalSpy spy(&login, SIGNAL(loggedIn(Login::Result)));
	login.login();
	QVERIFY(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	Login::Result result = arguments.at(0).value<Login::Result>();

	QCOMPARE(result, Login::Success);
}

void UrlLoginTest::testLoginFailure()
{
	MixedSettings *settings = m_site->settings();
	settings->setValue("login/type", "disabled");
	m_site->loadConfig();

	QList<AuthField*> fields;
	UrlAuth auth("url", fields, 10);
	UrlLogin login(&auth, m_site, &m_manager, settings);

	QVERIFY(login.isTestable());

	for (int i = 0; i < 3; ++i) {
		CustomNetworkAccessManager::NextFiles.enqueue("404");
	}

	QSignalSpy spy(&login, SIGNAL(loggedIn(Login::Result)));
	login.login();
	QVERIFY(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	Login::Result result = arguments.at(0).value<Login::Result>();

	QCOMPARE(result, Login::Failure);
}

void UrlLoginTest::testComplementUrl()
{
	QList<AuthField*> fields;
	fields.append(new AuthConstField("a", "1"));
	fields.append(new AuthConstField("b", "2"));

	UrlAuth auth("url", fields, 10);
	UrlLogin login(&auth, m_site, &m_manager, m_site->settings());

	QCOMPARE(login.complementUrl("/"), QString("/?a=1&b=2"));
	QCOMPARE(login.complementUrl("/?test=1"), QString("/?test=1&a=1&b=2"));
}


QTEST_MAIN(UrlLoginTest)
