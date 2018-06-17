#include "derpibooru-test.h"
#include <QStringList>
#include <QtTest>
#include "functions.h"


void DerpibooruTest::testHtml()
{
	QList<Image*> images = getImages("Booru-on-rails", "derpibooru.org", "regex", "safe", "results.html");

	// Convert results
	QList<qulonglong> ids;
	ids.reserve(images.count());
	for (Image *img : images)
	{
		ids.append(img->id());
	}

	// Check results
	ids = ids.mid(0, 3);
	QList<qulonglong> expected = QList<qulonglong>() << 1752855 << 1752854 << 1752853;
	QCOMPARE(images.count(), 15);
	QCOMPARE(ids, expected);
}

void DerpibooruTest::testJson()
{
	QList<Image*> images = getImages("Booru-on-rails", "derpibooru.org", "json", "safe", "results.json");

	// Convert results
	QList<qulonglong> ids;
	ids.reserve(images.count());
	for (Image *img : images)
	{
		ids.append(img->id());
	}

	// Check results
	ids = ids.mid(0, 3);
	QList<qulonglong> expected = QList<qulonglong>() << 1248664 << 1248663 << 1248661;
	QCOMPARE(images.count(), 15);
	QCOMPARE(ids, expected);
}

void DerpibooruTest::testHtmlTags()
{
	QList<Tag> tags = getTags("Booru-on-rails", "derpibooru.org", "regex", "tags.html");

	QCOMPARE(tags.count(), 250);

	QCOMPARE(tags[1].text(), QString("solo"));
	QCOMPARE(tags[1].count(), 599506);
	QCOMPARE(tags[1].type().isUnknown(), true);
}

void DerpibooruTest::testJsonTags()
{
	QList<Tag> tags = getTags("Booru-on-rails", "derpibooru.org", "json", "tags.json");

	QCOMPARE(tags.count(), 250);

	QCOMPARE(tags[1].text(), QString("solo"));
	QCOMPARE(tags[1].count(), 599506);
	QCOMPARE(tags[1].type().isUnknown(), true);
}


static DerpibooruTest instance;
