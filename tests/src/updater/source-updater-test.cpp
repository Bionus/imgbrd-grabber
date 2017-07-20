#include <QtTest>
#include "source-updater-test.h"
#include "updater/source-updater.h"
#include "models/source.h"


void SourceUpdaterTest::testNoUpdate()
{
	Source source(&profile, "tests/resources/sites/Danbooru (2.0)");
	SourceUpdater updater(&source, "http://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/release/sites");

	// Wait for updater
	QSignalSpy spy(&updater, SIGNAL(finished(Source*, bool)));
	updater.checkForUpdates();
	QVERIFY(spy.wait());

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	bool isNew = arguments.at(1).toBool();

	QVERIFY(!isNew);
}

void SourceUpdaterTest::testChanged()
{
	Source source(&profile, "tests/resources/sites/Danbooru");
	SourceUpdater updater(&source, "http://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/release/sites");

	// Wait for updater
	QSignalSpy spy(&updater, SIGNAL(finished(Source*, bool)));
	updater.checkForUpdates();
	QVERIFY(spy.wait());

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	bool isNew = arguments.at(1).toBool();

	QVERIFY(isNew);
}


static SourceUpdaterTest instance;
