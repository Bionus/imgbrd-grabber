#include "source-helpers.h"
#include <QDir>
#include <QFile>
#include <QSettings>
#include "models/profile.h"


Profile *makeProfile()
{
	QFile::remove("tests/resources/settings.ini");

	auto *profile = new Profile("tests/resources");
	profile->getSettings()->clear();

	return profile;
}

void setupSource(const QString &source, QString dir)
{
	if (dir.isEmpty()) {
		dir = "tests/resources/sites/";

		QFile::remove(dir + "helper.js");
		QFile("sites/helper.js").copy(dir + "helper.js");

		dir += source;
	}

	QDir().mkpath(dir);
	QFile::remove(dir + "/model.js");
	QFile::remove(dir + "/sites.txt");
	QFile("sites/" + source + "/model.js").copy(dir + "/model.js");
	QFile("sites/" + source + "/sites.txt").copy(dir + "/sites.txt");
}

void setupSite(const QString &source, const QString &site, QString dir)
{
	if (dir.isEmpty()) {
		dir = "tests/resources/sites/" + source + "/" + site;
	}

	QDir().mkpath(dir);
	QFile::remove(dir + "/defaults.ini");
	QFile::remove(dir + "/settings.ini");
	if (QFile::exists("sites/" + source + "/" + site + "/defaults.ini")) {
		QFile("sites/" + source + "/" + site + "/defaults.ini").copy(dir + "/defaults.ini");
	}
}
