#include "source-helpers.h"
#include <QDir>
#include <QFile>
#include <QSettings>
#include "models/profile.h"


Profile *makeProfile(bool clean)
{
	if (clean) {
		QFile::remove("tests/resources/blacklist.txt");
		QFile::remove("tests/resources/favorites.json");
		QFile::remove("tests/resources/favorites.txt");
		QFile::remove("tests/resources/ignore.txt");
		QFile::remove("tests/resources/md5s.sqlite");
		QFile::remove("tests/resources/md5s.txt");
		QFile::remove("tests/resources/viewitlater.txt");
		QFile::remove("tests/resources/wordsc.txt");
	}

	QFile settings("tests/resources/settings.ini");
	settings.open(QFile::Truncate | QFile::WriteOnly | QFile::Text);
	settings.close();

	auto *profile = new Profile("tests/resources");
	profile->getSettings()->clear();

	return profile;
}

void setupSource(const QString &source, QString dir)
{
	if (dir.isEmpty()) {
		dir = "tests/resources/sites/";

		QFile::remove(dir + "helper.js");
		QFile("../sites/helper.js").copy(dir + "helper.js");

		dir += source;
	}

	QDir().mkpath(dir);
	QFile::remove(dir + "/model.js");
	QFile::remove(dir + "/sites.txt");
	QFile("../sites/" + source + "/model.js").copy(dir + "/model.js");
	QFile("../sites/" + source + "/sites.txt").copy(dir + "/sites.txt");
}

void setupSite(const QString &source, const QString &site, QString dir)
{
	if (dir.isEmpty()) {
		dir = "tests/resources/sites/" + source + "/" + site;
	}

	QDir().mkpath(dir);
	QFile::remove(dir + "/defaults.ini");
	QFile::remove(dir + "/settings.ini");
	QFile::remove(dir + "/tag-types.txt");
	if (QFile::exists("../sites/" + source + "/" + site + "/defaults.ini")) {
		QFile("../sites/" + source + "/" + site + "/defaults.ini").copy(dir + "/defaults.ini");
	}
	if (QFile::exists("../sites/" + source + "/" + site + "/tag-types.txt")) {
		QFile("../sites/" + source + "/" + site + "/tag-types.txt").copy(dir + "/tag-types.txt");
	}
}
