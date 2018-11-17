#include "test-suite.h"
#include <QDir>
#include <QFile>


TestSuite::TestSuite()
	: QObject()
{
	getSuites().append(this);
}

void TestSuite::setupSource(const QString &source, QString dir)
{
	if (dir.isEmpty())
	{
		dir = "tests/resources/sites/";

		QFile::remove(dir + "helper.js");
		QFile("release/sites/helper.js").copy(dir + "helper.js");

		dir += source;
	}

	QDir().mkpath(dir);
	QFile::remove(dir + "/model.js");
	QFile::remove(dir + "/model.xml");
	QFile::remove(dir + "/sites.txt");
	QFile("release/sites/" + source + "/model.js").copy(dir + "/model.js");
	QFile("release/sites/" + source + "/model.xml").copy(dir + "/model.xml");
	QFile("release/sites/" + source + "/sites.txt").copy(dir + "/sites.txt");
}

void TestSuite::setupSite(const QString &source, const QString &site, QString dir)
{
	if (dir.isEmpty())
	{ dir = "tests/resources/sites/" + source + "/" + site; }

	QDir().mkpath(dir);
	QFile::remove(dir + "/defaults.ini");
	QFile::remove(dir + "/settings.ini");
	if (QFile::exists("release/sites/" + source + "/" + site + "/defaults.ini"))
	{ QFile("release/sites/" + source + "/" + site + "/defaults.ini").copy(dir + "/defaults.ini"); }
}

QList<TestSuite*> &TestSuite::getSuites()
{
	static QList<TestSuite*> suites;
	return suites;
}
