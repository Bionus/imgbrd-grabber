#include "derpibooru-test.h"
#include <QStringList>
#include <QtTest>
#include "functions.h"


void DerpibooruTest::testHtml()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Image*> images = getImages("Booru-on-rails", "derpibooru.org", "regex", "safe", "results.html", i != 0);

		// Convert results
		QList<qulonglong> ids;
		ids.reserve(images.count());
		for (Image *img : images)
		{
			ids.append(img->id());
		}

		// Check results
		ids = ids.mid(0, 3);
		QList<qulonglong> expected = QList<qulonglong>() << 1334191 << 1334187 << 1334186;
		QCOMPARE(images.count(), 15);
		QCOMPARE(ids, expected);
	}
}

void DerpibooruTest::testJson()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Image*> images = getImages("Booru-on-rails", "derpibooru.org", "json", "safe", "results.json", i != 0);

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
}

void DerpibooruTest::testHtmlTags()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Tag> tags = getTags("Booru-on-rails", "derpibooru.org", "regex", "tags.html", i != 0);

		QCOMPARE(tags.count(), 250);

		QCOMPARE(tags[1].text(), QString("solo"));
		QCOMPARE(tags[1].count(), 599506);
		QCOMPARE(tags[1].type().name(), QString("unknown"));
	}
}

void DerpibooruTest::testJsonTags()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Tag> tags = getTags("Booru-on-rails", "derpibooru.org", "json", "tags.json", i != 0);

		QCOMPARE(tags.count(), 250);

		QCOMPARE(tags[1].text(), QString("solo"));
		QCOMPARE(tags[1].count(), 599506);
		QCOMPARE(tags[1].type().name(), QString("unknown"));
	}
}


static DerpibooruTest instance;
