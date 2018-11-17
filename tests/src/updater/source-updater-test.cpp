#include "source-updater-test.h"
#include <QtTest>
#include "models/source.h"
#include "updater/source-updater.h"


#if defined(Q_OS_WIN) && 0
void SourceUpdaterTest::testNoUpdate()
{
	SourceUpdater updater("Danbooru (2.0)", "tests/resources/sites/Danbooru (2.0)", "http://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/release/sites");

	// Wait for updater
	QSignalSpy spy(&updater, SIGNAL(finished(QString, bool)));
	updater.checkForUpdates();
	QVERIFY(spy.wait());

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	bool isNew = arguments.at(1).toBool();

	QVERIFY(!isNew);
}

void SourceUpdaterTest::testChanged()
{
	SourceUpdater updater("Danbooru", "tests/resources/sites/Danbooru", "http://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/release/sites");

	// Wait for updater
	QSignalSpy spy(&updater, SIGNAL(finished(QString, bool)));
	updater.checkForUpdates();
	QVERIFY(spy.wait());

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	bool isNew = arguments.at(1).toBool();

	QVERIFY(isNew);
}
#endif


QTEST_MAIN(SourceUpdaterTest)
