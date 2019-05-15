#include "login/url-login-test.h"
#include <QSettings>
#include <QtTest>
#include "auth/auth-const-field.h"
#include "auth/auth-field.h"
#include "auth/url-auth.h"
#include "custom-network-access-manager.h"
#include "login/url-login.h"
#include "mixed-settings.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


void UrlLoginTest::init()
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	m_profile = makeProfile();
	m_site = m_profile->getSites().value("danbooru.donmai.us");
}

void UrlLoginTest::cleanup()
{
	m_profile->deleteLater();
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
	fields.append(new AuthConstField("b", ""));
	fields.append(new AuthConstField("c", "2"));

	UrlAuth auth("url", fields, 10);
	UrlLogin login(&auth, m_site, &m_manager, m_site->settings());

	QCOMPARE(login.complementUrl("/"), QString("/?a=1&c=2"));
	QCOMPARE(login.complementUrl("/?ho=&test=1"), QString("/?ho=&test=1&a=1&c=2"));
}


QTEST_MAIN(UrlLoginTest)
