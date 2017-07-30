#include <QtTest>
#include <QStringList>
#include "zerochan-test.h"


void ZerochanTest::testHtml()
{
	QList<Image*> images = getImages("Zerochan", "www.zerochan.net", "regex", "Touhou", "results.html");

	// Convert results
	QList<int> ids;
	for (Image *img : images)
	{
		ids.append(img->id());
	}

	// Check results
	ids = ids.mid(0, 3);
	QList<int> expected = QList<int>() << 2034435 << 2034432 << 2034431;
	QCOMPARE(images.count(), 20);
	QCOMPARE(ids, expected);
}

void ZerochanTest::testRss()
{
	QList<Image*> images = getImages("Zerochan", "www.zerochan.net", "rss", "Touhou", "results.rss");

	// Convert results
	QList<int> ids;
	for (Image *img : images)
	{
		ids.append(img->id());
	}

	// Check results
	ids = ids.mid(0, 3);
	QList<int> expected = QList<int>() << 2034435 << 2034432 << 2034431;
	QCOMPARE(images.count(), 20);
	QCOMPARE(ids, expected);
}


static ZerochanTest instance;
