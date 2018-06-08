#include "tag-api-test.h"
#include <QtTest>
#include "custom-network-access-manager.h"
#include "models/source.h"


void TagApiTest::init()
{
	m_profile = new Profile("tests/resources/");
	m_site = new Site("danbooru.donmai.us", new Source(m_profile, "release/sites/Danbooru (2.0)"));
}

void TagApiTest::cleanup()
{
	m_profile->deleteLater();
	m_site->deleteLater();
}


void TagApiTest::testBasic()
{
	TagApi tagApi(m_profile, m_site, m_site->getApis().first(), 1, 100);

	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.xml");

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
