#include "source-test.h"
#include <QtTest>


void SourceTest::init()
{
	QDir().mkpath("tests/resources/sites/tmp");

	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	m_profile = new Profile("tests/resources/");
	m_settings = m_profile->getSettings();
	m_source = new Source(m_profile, "tests/resources/sites/Danbooru (2.0)");
}

void SourceTest::cleanup()
{
	m_profile->deleteLater();
	m_source->deleteLater();
}


void SourceTest::testMissingXml()
{
	setupSource("Danbooru (2.0)", "tests/resources/sites/tmp/");
	QFile::remove("tests/resources/sites/tmp/model.xml");

	Source source(m_profile, "tests/resources/sites/tmp");
	QVERIFY(source.getApis().isEmpty());
}

void SourceTest::testMissingJavascript()
{
	setupSource("Danbooru (2.0)", "tests/resources/sites/tmp/");
	QFile::remove("tests/resources/sites/tmp/model.js");

	Source source(m_profile, "tests/resources/sites/tmp");
	QVERIFY(source.getApis().isEmpty());
}

void SourceTest::testInvalidXml()
{
	setupSource("Danbooru (2.0)", "tests/resources/sites/tmp/");

	QFile f("tests/resources/sites/tmp/model.xml");
	f.open(QFile::WriteOnly);
	f.write(QString("test").toUtf8());
	f.close();

	Source source(m_profile, "tests/resources/sites/tmp");
	QVERIFY(source.getApis().isEmpty());
}

void SourceTest::testInvalidJavascript()
{
	setupSource("Danbooru (2.0)", "tests/resources/sites/tmp/");

	QFile f("tests/resources/sites/tmp/model.js");
	f.open(QFile::WriteOnly);
	f.write(QString("test").toUtf8());
	f.close();

	Source source(m_profile, "tests/resources/sites/tmp");
	QVERIFY(source.getApis().isEmpty());
}

void SourceTest::testMissingSites()
{
	setupSource("Danbooru (2.0)", "tests/resources/sites/tmp/");

	QFile f("tests/resources/sites/tmp/sites.txt");
	f.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);
	f.write(QString("\n\n\r\ndanbooru.donmai.us\n").toUtf8());
	f.close();

	Source source(m_profile, "tests/resources/sites/tmp");
	QVERIFY(!source.getApis().isEmpty());
	QCOMPARE(source.getSites().count(), 1);
}

void SourceTest::testIgnoreEmptySites()
{
	setupSource("Danbooru (2.0)", "tests/resources/sites/tmp/");
	QFile::remove("tests/resources/sites/tmp/sites.txt");

	Source source(m_profile, "tests/resources/sites/tmp");
	QVERIFY(!source.getApis().isEmpty());
	QVERIFY(source.getSites().isEmpty());
}


static SourceTest instance;
