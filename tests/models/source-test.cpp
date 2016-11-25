#include <QtTest>
#include "source-test.h"


void SourceTest::init()
{
	QDir().mkpath("tests/resources/sites/tmp");
	QDir().mkpath("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us");
	QFile::remove("tests/resources/sites/Danbooru (2.0)/model.xml");
	QFile::remove("tests/resources/sites/Danbooru (2.0)/sites.txt");
	QFile::copy("release/sites/Danbooru (2.0)/model.xml", "tests/resources/sites/Danbooru (2.0)/model.xml");
	QFile::copy("release/sites/Danbooru (2.0)/sites.txt", "tests/resources/sites/Danbooru (2.0)/sites.txt");

	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);
	m_source = new Source(&profile, "tests/resources/sites/Danbooru (2.0)");
}

void SourceTest::cleanup()
{
	m_settings->deleteLater();
	m_source->deleteLater();
}


void SourceTest::testMissingXml()
{
	QFile::remove("tests/resources/sites/tmp/model.xml");

	Source source(&profile, "tests/resources/sites/tmp");
	QVERIFY(source.getApis().isEmpty());
}

void SourceTest::testInvalidXml()
{
	QFile f("tests/resources/sites/tmp/model.xml");
	f.open(QFile::WriteOnly);
	f.write(QString("test").toUtf8());
	f.close();

	Source source(&profile, "tests/resources/sites/tmp");
	QVERIFY(source.getApis().isEmpty());
}

void SourceTest::testMissingSites()
{
	QFile::remove("tests/resources/sites/tmp/model.xml");
	QFile::copy("release/sites/Danbooru (2.0)/model.xml", "tests/resources/sites/tmp/model.xml");
	QFile f("tests/resources/sites/tmp/sites.txt");
	f.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);
	f.write(QString("\n\n\r\ndanbooru.donmai.us\n").toUtf8());
	f.close();

	Source source(&profile, "tests/resources/sites/tmp");
	QVERIFY(!source.getApis().isEmpty());
	QCOMPARE(source.getSites().count(), 1);
}

void SourceTest::testIgnoreEmptySites()
{
	QFile::remove("tests/resources/sites/tmp/model.xml");
	QFile::remove("tests/resources/sites/tmp/sites.txt");
	QFile::copy("release/sites/Danbooru (2.0)/model.xml", "tests/resources/sites/tmp/model.xml");

	Source source(&profile, "tests/resources/sites/tmp");
	QVERIFY(!source.getApis().isEmpty());
	QVERIFY(source.getSites().isEmpty());
}

void SourceTest::testGetAllCached()
{
	QList<Source*> *sources1 = Source::getAllSources(&profile);
	QList<Source*> *sources2 = Source::getAllSources(&profile);

	QCOMPARE(sources1, sources2);
}


static SourceTest instance;
