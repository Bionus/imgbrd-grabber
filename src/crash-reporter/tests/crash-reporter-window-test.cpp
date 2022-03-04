#include <QFile>
#include <QLineEdit>
#include "catch.h"
#include "crash-reporter-window.h"
#include "raii-helpers.h"


TEST_CASE("CrashReporterWindow")
{
	FileDeleter lastDump("tests/resources/lastdump");

	SECTION("Renders default values")
	{
		const auto *window = new CrashReporterWindow(nullptr);

		const auto *lineLog = window->findChild<QLineEdit*>("lineLog");
		const auto *lineSettings = window->findChild<QLineEdit*>("lineSettings");
		const auto *lineDump = window->findChild<QLineEdit*>("lineDump");

		REQUIRE(lineLog->text().endsWith("main.log"));
		REQUIRE(lineSettings->text().endsWith("settings.ini"));
		REQUIRE(lineDump->text().isEmpty());
	}

	SECTION("Reads the 'lastdump' file if it exists")
	{
		QFile f("tests/resources/lastdump");
		f.open(QFile::WriteOnly | QFile::Truncate);
		f.write("test.dmp");
		f.close();

		const auto *window = new CrashReporterWindow(nullptr);

		const auto *lineDump = window->findChild<QLineEdit*>("lineDump");
		REQUIRE(lineDump->text().endsWith("test.dmp"));
	}
}
