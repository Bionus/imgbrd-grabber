#include <QtTest>
#include "source-test.h"


void SourceTest::init()
{
	QDir().mkpath("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us");
	QFile::remove("tests/resources/sites/Danbooru (2.0)/model.xml");
	QFile::remove("tests/resources/sites/Danbooru (2.0)/sites.txt");
	QFile::copy("release/sites/Danbooru (2.0)/model.xml", "tests/resources/sites/Danbooru (2.0)/model.xml");
	QFile::copy("release/sites/Danbooru (2.0)/sites.txt", "tests/resources/sites/Danbooru (2.0)/sites.txt");

	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);
	m_source = new Source("tests/resources/sites/Danbooru (2.0)");
}

void SourceTest::cleanup()
{
	m_settings->deleteLater();
	m_source->deleteLater();
}


void SourceTest::testGetAllCached()
{
	QList<Source*> *sources1 = Source::getAllSources();
	QList<Source*> *sources2 = Source::getAllSources();

	QCOMPARE(sources1, sources2);
}

void SourceTest::testCheckForUpdates()
{
	// Wait for tags
	QSignalSpy spy(m_source, SIGNAL(checkForUpdatesFinished(Source*)));
	m_source->checkForUpdates("https://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/release/sites/");
	QVERIFY(spy.wait());

	// Check result
	QVERIFY(!m_source->getUpdateVersion().isEmpty());
}


static SourceTest instance;
