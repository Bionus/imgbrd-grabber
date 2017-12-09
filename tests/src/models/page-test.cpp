#include <QtTest>
#include "page-test.h"
#include "models/source.h"


void PageTest::init()
{
	m_sites.append(new Site("danbooru.donmai.us", new Source(&profile, "release/sites/Danbooru (2.0)")));
	m_site = new Site("gelbooru.com", new Source(&profile, "release/sites/Gelbooru (0.2)"));
}


void PageTest::testIncompatibleModifiers()
{
	Page page(&profile, m_site, m_sites, QStringList() << "test" << "status:deleted");

	QCOMPARE(page.search().count(), 1);
	QCOMPARE(page.search().first(), QString("test"));
}

void PageTest::testLoadAbort()
{
	Page page(&profile, m_site, m_sites, QStringList() << "test" << "status:deleted");

	QSignalSpy spy(&page, SIGNAL(finishedLoading(Page*)));
	page.load();
	page.abort();
	QVERIFY(!spy.wait(1000));
}

void PageTest::testLoadTagsAbort()
{
	Page page(&profile, m_site, m_sites, QStringList() << "test" << "status:deleted");

	QSignalSpy spy(&page, SIGNAL(finishedLoadingTags(Page*)));
	page.loadTags();
	page.abortTags();
	QVERIFY(!spy.wait(1000));
}


static PageTest instance;
