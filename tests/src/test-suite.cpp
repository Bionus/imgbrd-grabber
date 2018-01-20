#include "test-suite.h"


TestSuite::TestSuite()
	: QObject(), profile("tests/resources/")
{
	getSuites().append(this);
}

void TestSuite::setupSource(const QString &site)
{
	QDir().mkpath("tests/resources/sites/" + site);
	QFile::remove("tests/resources/sites/" + site +"/model.xml");
	QFile("release/sites/" + site +"/model.xml").copy("tests/resources/sites/" + site +"/model.xml");
}

void TestSuite::setupSite(const QString &site, const QString &source)
{
	QDir().mkpath("tests/resources/sites/" + site + "/" + source);
	QFile::remove("tests/resources/sites/" + site +"/" + source + "/defaults.ini");
	QFile::remove("tests/resources/sites/" + site +"/" + source + "/settings.ini");
	if (QFile::exists("release/sites/" + site +"/" + source + "/defaults.ini"))
	{ QFile("release/sites/" + site +"/" + source + "/defaults.ini").copy("tests/resources/sites/" + site +"/" + source + "/defaults.ini"); }
}

QList<QObject*> &TestSuite::getSuites()
{
	static QList<QObject*> suites;
	return suites;
}
