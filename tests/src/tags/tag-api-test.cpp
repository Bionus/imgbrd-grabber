#include <QtTest>
#include "tag-api-test.h"
#include "models/source.h"


void TagApiTest::init()
{
	m_sites.insert("danbooru", new Site("danbooru.donmai.us", new Source(&profile, "release/sites/Danbooru (2.0)")));
	m_sites.insert("e621", new Site("e621.net", new Source(&profile, "release/sites/Danbooru")));
	m_sites.insert("behoimi", new Site("behoimi.org", new Source(&profile, "release/sites/Danbooru")));
}

void TagApiTest::cleanup()
{
	for (Site *site : m_sites)
		site->deleteLater();
}


void TagApiTest::testBasic()
{
	Site *site = m_sites["danbooru"];
	TagApi tagApi(&profile, site, site->getApis().first(), 1, 100);

	// Wait for downloader
	QSignalSpy spy(&tagApi, SIGNAL(finishedLoading(TagApi*, TagApi::LoadResult)));
	tagApi.load(false);
	QVERIFY(spy.wait());

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	TagApi::LoadResult result = arguments.at(1).value<TagApi::LoadResult>();

	QCOMPARE(result, TagApi::LoadResult::Ok);
	QCOMPARE(tagApi.tags().count(), 100);
}


static TagApiTest instance;
