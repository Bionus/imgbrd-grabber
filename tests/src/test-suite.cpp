#include "test-suite.h"
#include <QDir>
#include <QFile>
#include <QSettings>
#include "models/profile.h"


TestSuite::TestSuite()
{
	getSuites().append(this);
}

Profile *TestSuite::makeProfile() const
{
	QFile::remove("tests/resources/settings.ini");

	Profile *profile = new Profile("tests/resources");
	profile->getSettings()->clear();

	return profile;
}

void TestSuite::setupSource(const QString &source, QString dir) const
{
	if (dir.isEmpty()) {
		dir = "tests/resources/sites/";

		QFile::remove(dir + "helper.js");
		QFile("release/sites/helper.js").copy(dir + "helper.js");

		dir += source;
	}

	QDir().mkpath(dir);
	QFile::remove(dir + "/model.js");
	QFile::remove(dir + "/sites.txt");
	QFile("release/sites/" + source + "/model.js").copy(dir + "/model.js");
	QFile("release/sites/" + source + "/sites.txt").copy(dir + "/sites.txt");
}

void TestSuite::setupSite(const QString &source, const QString &site, QString dir) const
{
	if (dir.isEmpty()) {
		dir = "tests/resources/sites/" + source + "/" + site;
	}

	QDir().mkpath(dir);
	QFile::remove(dir + "/defaults.ini");
	QFile::remove(dir + "/settings.ini");
	if (QFile::exists("release/sites/" + source + "/" + site + "/defaults.ini")) {
		QFile("release/sites/" + source + "/" + site + "/defaults.ini").copy(dir + "/defaults.ini");
	}
}

QList<TestSuite*> &TestSuite::getSuites()
{
	static QList<TestSuite*> suites;
	return suites;
}
