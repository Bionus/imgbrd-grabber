#include "pack-loader-test.h"
#include <QList>
#include <QtTest>
#include "custom-network-access-manager.h"
#include "loader/pack-loader.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


void PackLoaderTest::init()
{
	m_profile = new Profile("tests/resources/");
}

void PackLoaderTest::cleanup()
{
	m_profile->deleteLater();
}


void PackLoaderTest::testGetQuery()
{
	DownloadQueryGroup query(QStringList() << "search", 1, 10, 20, QStringList(), false, nullptr, "%md5%.%ext%", "");
	PackLoader loader(nullptr, query, 100, nullptr);

	QCOMPARE(loader.query(), query);
}

void PackLoaderTest::testBasic()
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	Source source(m_profile, "tests/resources/sites/Danbooru (2.0)");
	Site site("danbooru.donmai.us", &source);

	// Login first
	QSignalSpy spy(&site, SIGNAL(loggedIn(Site*, Site::LoginResult)));
	QTimer::singleShot(0, &site, SLOT(login()));
	QVERIFY(spy.wait());

	// 2 packs of 9 from 8 pages of 2
	for (int i = 1; i <= 8; ++i) {
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/pack-loader-2-" + QString::number(i) + ".xml");
	}
	QCOMPARE(getResults(m_profile, &site, "filesize:<200KB", 2, 15, 9, true), QList<int>() << 9 << 6);

	// 5 packs of 3 from 7 pages of 2
	for (int i = 1; i <= 7; ++i) {
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/pack-loader-2-" + QString::number(i) + ".xml");
	}
	QCOMPARE(getResults(m_profile, &site, "filesize:<200KB", 2, 13, 3, true), QList<int>() << 3 << 3 << 3 << 3 << 1);

	// 3 packs of 6 from 1 page of 20
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/pack-loader-20-1.xml");
	QCOMPARE(getResults(m_profile, &site, "filesize:<200KB", 20, 15, 6, true), QList<int>() << 6 << 6 << 3);

	// 1 pack of 100 from 3 pages of 1
	for (int i = 1; i <= 3; ++i) {
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/pack-loader-1-" + QString::number(i) + ".xml");
	}
	QCOMPARE(getResults(m_profile, &site, "filesize:<200KB", 1, 3, 100, true), QList<int>() << 3);
}

void PackLoaderTest::testWrongResultsCount()
{
	setupSource("Gelbooru (0.2)");
	setupSite("Gelbooru (0.2)", "gelbooru.com");

	Source source(m_profile, "tests/resources/sites/Gelbooru (0.2)");
	Site site("gelbooru.com", &source);

	// Login first
	QSignalSpy spy(&site, SIGNAL(loggedIn(Site*, Site::LoginResult)));
	QTimer::singleShot(0, &site, SLOT(login()));
	QVERIFY(spy.wait());

	// 4 packs of 90 from 7 pages of 50
	for (int i = 1; i <= 7; ++i) {
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/gelbooru.com/pack-loader-" + QString::number(i) + ".html");
	}
	QCOMPARE(getResults(m_profile, &site, "fav:123", 20, 400, 90, true), QList<int>() << 90 << 90 << 90 << 64);

	// 5 packs of 30 from 3 pages of 50
	for (int i = 1; i <= 3; ++i) {
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/gelbooru.com/pack-loader-" + QString::number(i) + ".html");
	}
	QCOMPARE(getResults(m_profile, &site, "fav:123", 20, 140, 30, true), QList<int>() << 30 << 30 << 30 << 30 << 20);

	// 3 packs of 50 from 3 pages of 50
	for (int i = 1; i <= 3; ++i) {
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/gelbooru.com/pack-loader-" + QString::number(i) + ".html");
	}
	QCOMPARE(getResults(m_profile, &site, "fav:123", 200, 140, 50, true), QList<int>() << 50 << 50 << 40);

	// 1 pack of 5 from 1 page of 50
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/gelbooru.com/pack-loader-1.html");
	QCOMPARE(getResults(m_profile, &site, "fav:123", 1, 5, 100, true), QList<int>() << 5);
}

void PackLoaderTest::testGalleries()
{
	setupSource("E-Hentai");
	setupSite("E-Hentai", "e-hentai.org");

	Source source(m_profile, "tests/resources/sites/E-Hentai");
	Site site("e-hentai.org", &source);

	// Login first
	QSignalSpy spy(&site, SIGNAL(loggedIn(Site*, Site::LoginResult)));
	QTimer::singleShot(0, &site, SLOT(login()));
	QVERIFY(spy.wait());

	// 2 packs of 30 from 1 gallery of 31
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/e-hentai.org/pack-loader-list.html");
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/e-hentai.org/pack-loader-gallery-1-1.html");
	QCOMPARE(getResults(m_profile, &site, "tomose shunsaku", 1, 1, 30, true), QList<int>() << 30 << 1);

	// 2 packs of 50 from 3 galleries of 31, 32, 24
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/e-hentai.org/pack-loader-list.html");
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/e-hentai.org/pack-loader-gallery-1-1.html");
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/e-hentai.org/pack-loader-gallery-2-1.html");
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/e-hentai.org/pack-loader-gallery-3-1.html");
	QCOMPARE(getResults(m_profile, &site, "tomose shunsaku", 1, 3, 50, true), QList<int>() << 50 << 37);

	// 1 pack of 50 from 1 gallery of 31 (images)
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/e-hentai.org/pack-loader-list.html");
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/e-hentai.org/pack-loader-gallery-1-1.html");
	QCOMPARE(getResults(m_profile, &site, "tomose shunsaku", 1, 3, 50, false), QList<int>() << 3);
}


QList<int> PackLoaderTest::getResults(Profile *profile, Site *site, QString search, int perPage, int total, int packSize, bool galleriesCountAsOne)
{
	QList<int> ret;

	DownloadQueryGroup query(QStringList() << search, 1, perPage, total, QStringList(), false, site, "%md5%.%ext%", "");
	query.galleriesCountAsOne = galleriesCountAsOne;

	PackLoader loader(profile, query, packSize, nullptr);
	loader.start();
	while (loader.hasNext()) {
		auto images = loader.next();
		ret.append(images.count());
	}

	return ret;
}


QTEST_MAIN(PackLoaderTest)
