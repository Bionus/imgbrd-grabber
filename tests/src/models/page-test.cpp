#include "page-test.h"
#include <QtTest>
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


void PageTest::init()
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	setupSource("Gelbooru (0.2)");
	setupSite("Gelbooru (0.2)", "gelbooru.com");

	m_profile = new Profile("tests/resources/");
	m_sites.append(m_profile->getSites().value("danbooru.donmai.us"));
	m_site = m_profile->getSites().value("gelbooru.com");
}

void PageTest::cleanup()
{
	m_profile->deleteLater();
	m_sites.clear();
}


void PageTest::testIncompatibleModifiers()
{
	Page page(m_profile, m_site, m_sites, QStringList() << "test" << "status:deleted");

	QCOMPARE(page.search().count(), 1);
	QCOMPARE(page.search().first(), QString("test"));
}

void PageTest::testLoadAbort()
{
	Page page(m_profile, m_site, m_sites, QStringList() << "test" << "status:deleted");

	QSignalSpy spy(&page, SIGNAL(finishedLoading(Page*)));
	page.load();
	page.abort();
	QVERIFY(!spy.wait(1000));
}

void PageTest::testLoadTagsAbort()
{
	Page page(m_profile, m_site, m_sites, QStringList() << "test" << "status:deleted");

	QSignalSpy spy(&page, SIGNAL(finishedLoadingTags(Page*)));
	page.loadTags();
	page.abortTags();
	QVERIFY(!spy.wait(1000));
}


QTEST_MAIN(PageTest)
