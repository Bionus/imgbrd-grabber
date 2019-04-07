#include "monitor-test.h"
#include <QtTest>
#include "models/monitor.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


void MonitorTest::init()
{
	m_profile = new Profile("tests/resources/settings.ini");
	m_source = new Source(m_profile, "release/sites/Danbooru (2.0)");
	m_site = new Site("danbooru.donmai.us", m_source);
}

void MonitorTest::cleanup()
{
	m_profile->deleteLater();
	m_source->deleteLater();
	m_site->deleteLater();
}


void MonitorTest::testSite()
{
    Monitor monitor(m_site, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);

	QCOMPARE(monitor.site(), m_site);
}

void MonitorTest::testInterval()
{
    Monitor monitor(m_site, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);

	QCOMPARE(monitor.interval(), 60);
}

void MonitorTest::testLastCheck()
{
	QDateTime before(QDate(2016, 7, 2), QTime(16, 35, 12));
	QDateTime after(QDate(2018, 7, 2), QTime(16, 35, 12));

    Monitor monitor(m_site, 60, before, false, "", "", 12, true);

	QCOMPARE(monitor.lastCheck(), before);
	monitor.setLastCheck(after);
	QCOMPARE(monitor.lastCheck(), after);
}

void MonitorTest::testCumulated()
{
    Monitor monitor(m_site, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);

	QCOMPARE(monitor.cumulated(), 12);
	QCOMPARE(monitor.preciseCumulated(), true);
	monitor.setCumulated(20, false);
	QCOMPARE(monitor.cumulated(), 20);
	QCOMPARE(monitor.preciseCumulated(), false);
}

void MonitorTest::testSerialization()
{
    Monitor original(m_site, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);

	QJsonObject json;
	original.toJson(json);

	Monitor dest = Monitor::fromJson(json, QMap<QString, Site*> {{ m_site->url(), m_site }});

	QCOMPARE(dest.site(), original.site());
	QCOMPARE(dest.interval(), original.interval());
	QCOMPARE(dest.lastCheck(), original.lastCheck());
	QCOMPARE(dest.cumulated(), original.cumulated());
	QCOMPARE(dest.preciseCumulated(), original.preciseCumulated());
}

void MonitorTest::testCompare()
{
    Monitor a(m_site, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);
    Monitor b(m_site, 60, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);
    Monitor c(m_site, 120, QDateTime(QDate(2016, 7, 2), QTime(16, 35, 12)), false, "", "", 12, true);

	QVERIFY(a == b);
	QVERIFY(b == a);
	QVERIFY(a != c);
	QVERIFY(b != c);
	QVERIFY(c == c);
}


QTEST_MAIN(MonitorTest)
