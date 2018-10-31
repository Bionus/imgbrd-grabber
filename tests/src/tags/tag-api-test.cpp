#include "tag-api-test.h"
#include <QtTest>
#include "custom-network-access-manager.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "tags/tag.h"
#include "tags/tag-api.h"


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


TagApi::LoadResult load(TagApi *api)
{
	// Wait for downloader
	QSignalSpy spy(api, SIGNAL(finishedLoading(TagApi*, TagApi::LoadResult)));
	api->load(false);
	if (!spy.wait())
		return TagApi::LoadResult::Error;

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	TagApi::LoadResult result = arguments.at(1).value<TagApi::LoadResult>();

	return result;
}

void TagApiTest::testBasic()
{
	TagApi tagApi(m_profile, m_site, m_site->getApis().first(), 1, 100);
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.xml");

	TagApi::LoadResult result = load(&tagApi);

	QCOMPARE(result, TagApi::LoadResult::Ok);
	QCOMPARE(tagApi.tags().count(), 100);
	QCOMPARE(tagApi.tags().at(1).text(), QString("walkr"));
	QCOMPARE(tagApi.tags().at(1).type().name(), QString("copyright"));
}

void TagApiTest::testNetworkError()
{
	TagApi tagApi(m_profile, m_site, m_site->getApis().first(), 1, 100);
	CustomNetworkAccessManager::NextFiles.enqueue("404");

	TagApi::LoadResult result = load(&tagApi);

	QCOMPARE(result, TagApi::LoadResult::Error);
	QCOMPARE(tagApi.tags().count(), 0);
}

void TagApiTest::testParseError()
{
	TagApi tagApi(m_profile, m_site, m_site->getApis().first(), 1, 100);
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.html");

	TagApi::LoadResult result = load(&tagApi);

	QCOMPARE(result, TagApi::LoadResult::Error);
	QCOMPARE(tagApi.tags().count(), 0);
}

void TagApiTest::testDoubleLoad()
{
	TagApi tagApi(m_profile, m_site, m_site->getApis().first(), 1, 100);
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.xml");
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.xml");

	load(&tagApi);
	TagApi::LoadResult result = load(&tagApi);

	QCOMPARE(result, TagApi::LoadResult::Ok);
}

void TagApiTest::testRedirect()
{
	TagApi tagApi(m_profile, m_site, m_site->getApis().first(), 1, 100);
	CustomNetworkAccessManager::NextFiles.enqueue("redirect");
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/tags.xml");

	TagApi::LoadResult result = load(&tagApi);

	QCOMPARE(result, TagApi::LoadResult::Ok);
}

void TagApiTest::testAbort()
{
	TagApi tagApi(m_profile, m_site, m_site->getApis().first(), 1, 100);

	QSignalSpy spy(&tagApi, SIGNAL(finishedLoading(TagApi*, TagApi::LoadResult)));
	tagApi.load(false);
	tagApi.abort();
	QVERIFY(!spy.wait(1000));
}


QTEST_MAIN(TagApiTest)
