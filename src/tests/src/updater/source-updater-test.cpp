#include <QSignalSpy>
#include "models/source.h"
#include "updater/source-updater.h"
#include "catch.h"


#if defined(Q_OS_WIN) && 0
TEST_CASE("SourceUpdater")
{
	SECTION("No update")
	{
		SourceUpdater updater("Danbooru (2.0)", "tests/resources/sites/Danbooru (2.0)", "http://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/release/sites");

		// Wait for updater
		QSignalSpy spy(&updater, SIGNAL(finished(QString, bool)));
		updater.checkForUpdates();
		REQUIRE(spy.wait());

		// Get results
		QList<QVariant> arguments = spy.takeFirst();
		bool isNew = arguments.at(1).toBool();

		REQUIRE(!isNew);
	}

	SECTION("Changed")
	{
		SourceUpdater updater("Danbooru", "tests/resources/sites/Danbooru", "http://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/release/sites");

		// Wait for updater
		QSignalSpy spy(&updater, SIGNAL(finished(QString, bool)));
		updater.checkForUpdates();
		REQUIRE(spy.wait());

		// Get results
		QList<QVariant> arguments = spy.takeFirst();
		bool isNew = arguments.at(1).toBool();

		REQUIRE(isNew);
	}
}
#endif
