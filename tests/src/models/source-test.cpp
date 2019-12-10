#include <QDir>
#include <QScopedPointer>
#include "models/profile.h"
#include "models/source.h"
#include "catch.h"
#include "raii-helpers.h"
#include "source-helpers.h"


TEST_CASE("Source")
{
	DirectoryDeleter tmp("tests/resources/sites/tmp");

	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

	SECTION("MissingJavascript")
	{
		setupSource("Danbooru (2.0)", "tests/resources/sites/tmp/");
		QFile::remove("tests/resources/sites/tmp/model.js");

		Source source(profile, "tests/resources/sites/tmp");
		REQUIRE(source.getApis().isEmpty());
	}

	SECTION("InvalidJavascript")
	{
		setupSource("Danbooru (2.0)", "tests/resources/sites/tmp/");

		QFile f("tests/resources/sites/tmp/model.js");
		f.open(QFile::WriteOnly);
		f.write(QString("test").toUtf8());
		f.close();

		Source source(profile, "tests/resources/sites/tmp");
		REQUIRE(source.getApis().isEmpty());
	}

	SECTION("MissingSites")
	{
		setupSource("Danbooru (2.0)", "tests/resources/sites/tmp/");

		QFile f("tests/resources/sites/tmp/sites.txt");
		f.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);
		f.write(QString("\n\n\r\ndanbooru.donmai.us\n").toUtf8());
		f.close();

		Source source(profile, "tests/resources/sites/tmp");
		REQUIRE(!source.getApis().isEmpty());
		REQUIRE(source.getSites().count() == 1);
	}

	SECTION("IgnoreEmptySites")
	{
		setupSource("Danbooru (2.0)", "tests/resources/sites/tmp/");
		QFile::remove("tests/resources/sites/tmp/sites.txt");

		Source source(profile, "tests/resources/sites/tmp");
		REQUIRE(!source.getApis().isEmpty());
		REQUIRE(source.getSites().isEmpty());
	}
}
