#include "source-guesser-test.h"
#include <QtTest>
#include "custom-network-access-manager.h"
#include "models/profile.h"
#include "models/source.h"
#include "models/source-guesser.h"


void SourceGuesserTest::initTestCase()
{
	setupSource("Danbooru");
	setupSource("Danbooru (2.0)");

	m_profile = new Profile("tests/resources/");
}

void SourceGuesserTest::cleanupTestCase()
{
	m_profile->deleteLater();
}


void SourceGuesserTest::testNotFound()
{
	QList<Source*> sources;
	sources.append(new Source(m_profile, "tests/resources/sites/Danbooru"));

	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/homepage.html");

	SourceGuesser guesser("https://danbooru.donmai.us", sources);
	Source *source = guesser.start();

	QVERIFY(source == nullptr);
}

void SourceGuesserTest::testNetworkError()
{
	QList<Source*> sources;
	sources.append(new Source(m_profile, "tests/resources/sites/Danbooru"));

	CustomNetworkAccessManager::NextFiles.enqueue("404");

	SourceGuesser guesser("http://behoimi.org", sources);
	Source *source = guesser.start();

	QVERIFY(source == nullptr);
}

void SourceGuesserTest::testDanbooru1()
{
	QList<Source*> sources;
	sources.append(new Source(m_profile, "tests/resources/sites/Danbooru"));

	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/behoimi.org/homepage.html");

	SourceGuesser guesser("http://behoimi.org", sources);
	Source *source = guesser.start();

	QVERIFY(source != nullptr);
	QCOMPARE(source->getName(), QString("Danbooru"));
}

void SourceGuesserTest::testDanbooru2()
{
	QList<Source*> sources;
	sources.append(new Source(m_profile, "tests/resources/sites/Danbooru (2.0)"));

	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/homepage.html");

	SourceGuesser guesser("https://danbooru.donmai.us", sources);
	Source *source = guesser.start();

	QVERIFY(source != nullptr);
	QCOMPARE(source->getName(), QString("Danbooru (2.0)"));
}


QTEST_MAIN(SourceGuesserTest)
