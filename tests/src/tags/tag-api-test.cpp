#include <QtTest>
#include "tag-api-test.h"
#include "models/source.h"


void TagApiTest::init()
{
	m_site = new Site("danbooru.donmai.us", new Source(&profile, "release/sites/Danbooru (2.0)"));
}

void TagApiTest::cleanup()
{
	m_site->deleteLater();
}


void TagApiTest::testBasic()
{
	TagApi tagApi(&profile, m_site, m_site->getApis().first(), 1, 100);

	// Wait for downloader
	QSignalSpy spy(&tagApi, SIGNAL(finishedLoading(TagApi*, TagApi::LoadResult)));
	tagApi.load(false);
	QVERIFY(spy.wait());

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	TagApi::LoadResult result = arguments.at(1).value<TagApi::LoadResult>();

	QCOMPARE(result, TagApi::LoadResult::Ok);
	QCOMPARE(tagApi.tags().count(), 100);
	QCOMPARE(tagApi.tags().at(1).text(), QString("walkr"));
	QCOMPARE(tagApi.tags().at(1).type().name(), QString("copyright"));
}


static TagApiTest instance;
