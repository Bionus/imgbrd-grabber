#include "test-suite.h"


TestSuite::TestSuite()
	: QObject()
{
	getSuites().append(this);
}

void TestSuite::setupSource(const QString &site)
{
	QFile::remove("tests/resources/sites/helper.js");
	QFile("release/sites/helper.js").copy("tests/resources/sites/helper.js");

	QDir().mkpath("tests/resources/sites/" + site);
	QFile::remove("tests/resources/sites/" + site + "/sites.txt");
	QFile::remove("tests/resources/sites/" + site +"/model.xml");
	QFile::remove("tests/resources/sites/" + site +"/model.js");
	QFile("release/sites/" + site +"/sites.txt").copy("tests/resources/sites/" + site +"/sites.txt");
	QFile("release/sites/" + site +"/model.xml").copy("tests/resources/sites/" + site +"/model.xml");
	QFile("release/sites/" + site +"/model.js").copy("tests/resources/sites/" + site +"/model.js");
}

void TestSuite::setupSite(const QString &site, const QString &source)
{
	QDir().mkpath("tests/resources/sites/" + site + "/" + source);
	QFile::remove("tests/resources/sites/" + site +"/" + source + "/defaults.ini");
	QFile::remove("tests/resources/sites/" + site +"/" + source + "/settings.ini");
	if (QFile::exists("release/sites/" + site +"/" + source + "/defaults.ini"))
	{ QFile("release/sites/" + site +"/" + source + "/defaults.ini").copy("tests/resources/sites/" + site +"/" + source + "/defaults.ini"); }
}

QList<TestSuite*> &TestSuite::getSuites()
{
	static QList<TestSuite*> suites;
	return suites;
}
