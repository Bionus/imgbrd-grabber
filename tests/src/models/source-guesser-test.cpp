#include "source-guesser-test.h"
#include "models/source-guesser.h"
#include "models/source.h"


void SourceGuesserTest::initTestCase()
{
	setupSource("Danbooru");
	setupSource("Danbooru (2.0)");
}

void SourceGuesserTest::testNotFound()
{
	QList<Source*> sources;
	sources.append(new Source(&profile, "tests/resources/sites/Danbooru"));

	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/homepage.html");

	SourceGuesser guesser("https://danbooru.donmai.us", sources);
	Source *source = guesser.start();

	Q_ASSERT(source == nullptr);
}

void SourceGuesserTest::testDanbooru1()
{
	QList<Source*> sources;
	sources.append(new Source(&profile, "tests/resources/sites/Danbooru"));

	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/behoimi.org/homepage.html");

	SourceGuesser guesser("http://behoimi.org", sources);
	Source *source = guesser.start();

	Q_ASSERT(source != nullptr);
	QCOMPARE(source->getName(), QString("Danbooru"));
}

void SourceGuesserTest::testDanbooru2()
{
	QList<Source*> sources;
	sources.append(new Source(&profile, "tests/resources/sites/Danbooru (2.0)"));

	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/homepage.html");

	SourceGuesser guesser("https://danbooru.donmai.us", sources);
	Source *source = guesser.start();

	Q_ASSERT(source != nullptr);
	QCOMPARE(source->getName(), QString("Danbooru (2.0)"));
}


static SourceGuesserTest instance;
